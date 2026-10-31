#include "sc_compiler/sc_compiler.hpp"
#include "sc_parser/sc_grammar_shared.hpp"
#include "sc_source/normalise_source.hpp"
#include "sc_source/text_info.hpp"
#include "sc_source/text_location.hpp"

#include <algorithm>
#include <charconv>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <thread>
#include <variant>
#include <vector>

// Times each stage of compiling interpreted code against a compiled class library, in this process, many times per
// snippet, as one compile takes microseconds:
//
//     sc_bench_interpreted [--runs n] [--threads n] [--interpreted-version x.y] [dir|.sc file]... [.scd file]...
//
// Each dir or .sc file is a module of the class library, compiled once on --threads, then timed end to end. Each scd
// file, or the built in snippets without one, is compiled --runs times after a few untimed ones, printing the median
// and fastest of each stage in microseconds. Use a Release build. The VM's loader isn't in langutils, so isn't timed.

namespace {

////////////////////////////////////////////////////////////////////////////////
// Snippets
////////////////////////////////////////////////////////////////////////////////

struct Snippet {
    std::string name;
    std::string source;
};

constexpr const char* one_liner = R"sc({ SinOsc.ar([440, 442], 0, 0.1) }.play;)sc";

constexpr const char* few_functions = R"sc((
var notes = [60, 62, 64, 65, 67];
var scale = { |n, root = 0| (n + root).midicps };
var chords = notes.collect { |n, i| [n, n + 4, n + 7].collect { |m| scale.(m, i) } };
chords.do { |c| if (c.sum > 1000) { c.postln } { "low".postln } };
)
)sc";

constexpr const char* large_block = R"sc((
s.waitForBoot {
    SynthDef(\pluck, { |out = 0, freq = 440, amp = 0.2, pan = 0, decay = 2, gate = 1|
        var exc = PinkNoise.ar(amp) * EnvGen.kr(Env.perc(0.001, 0.01));
        var sig = CombL.ar(exc, 0.05, freq.reciprocal, decay);
        var env = EnvGen.kr(Env.asr(0.001, 1, 0.3), gate, doneAction: 2);
        sig = LPF.ar(sig, (freq * 6).clip(200, 18000));
        Out.ar(out, Pan2.ar(sig * env, pan));
    }).add;

    SynthDef(\pad, { |out = 0, freq = 220, amp = 0.1, atk = 2, rel = 4, gate = 1, cutoff = 1200|
        var detune = [0.995, 1, 1.005, 1.01];
        var sig = Saw.ar(freq * detune).sum * 0.25;
        var env = EnvGen.kr(Env.adsr(atk, 0.5, 0.7, rel), gate, doneAction: 2);
        sig = RLPF.ar(sig, cutoff * LFNoise2.kr(0.3).range(0.5, 2), 0.3);
        sig = Splay.ar([sig, DelayC.ar(sig, 0.02, 0.013)]);
        Out.ar(out, sig * env * amp);
    }).add;

    s.sync;

    ~root = 48;
    ~scale = Scale.dorian;
    ~degrees = (0..7) ++ (7..0);
    ~toFreq = { |degree, octave = 0| (~scale.degreeToKey(degree) + ~root + (12 * octave)).midicps };

    ~melody = Pbind(
        \instrument, \pluck,
        \freq, Pfunc { |ev| ~toFreq.(~degrees.choose, [1, 2].choose) },
        \dur, Pseq([0.25, 0.25, 0.5, 0.125, 0.125, 0.75], inf),
        \amp, Pwhite(0.1, 0.3),
        \pan, Pseg([-0.7, 0.7, -0.7], [8, 8], \sin, inf),
        \decay, Prand([1, 2, 4], inf)
    );

    ~chords = Routine {
        var progression = [[0, 2, 4], [3, 5, 7], [4, 6, 8], [1, 3, 5]];
        var synths = [];
        loop {
            progression.do { |chord, i|
                synths.do { |x| x.set(\gate, 0) };
                synths = chord.collect { |degree|
                    Synth(\pad, [\freq, ~toFreq.(degree), \cutoff, 600 + (i * 300), \amp, 0.05]);
                };
                if (i.odd) { 4.wait } { 8.wait };
            };
        };
    };

    ~analyse = { |values|
        var sum = 0, count = 0, peak = -inf, histogram = Dictionary.new;
        values.do { |v|
            var bucket = (v / 10).floor.asInteger;
            sum = sum + v;
            count = count + 1;
            peak = max(peak, v);
            histogram[bucket] = (histogram[bucket] ? 0) + 1;
        };
        (mean: if (count > 0) { sum / count } { 0 }, peak: peak, histogram: histogram)
    };

    ~report = { |stats|
        var keys = stats[\histogram].keys.asArray.sort;
        "mean % peak %".format(stats[\mean].round(0.01), stats[\peak]).postln;
        keys.do { |k| (k * 10).asString.padLeft(4).post; " ".post; ("#" ! stats[\histogram][k]).join.postln };
    };

    ~report.(~analyse.((1..200).collect { |i| (i * 7 % 97) + 3.rand }));

    ~player = ~melody.play(TempoClock.default, quant: 4);
    ~chords.play(TempoClock.default, quant: 4);

    Task {
        var step = 0;
        while { step < 16 } {
            ~root = [48, 50, 53, 45].wrapAt(step);
            step = step + 1;
            16.wait;
        };
        ~player.stop;
        ~chords.stop;
        "done".postln;
    }.play;
};
)
)sc";

// Each level reads and assigns the variables of every level around it.
constexpr const char* deeply_nested = R"sc((
var a1 = 1, b1 = 2, c1 = 3, d1 = 4, e1 = 5, f1 = 6;
(1..4).collect { |x2, i2|
    var a2 = x2 + a1, b2 = i2 * b1, c2 = c1 - x2, d2 = d1 + a2, e2 = e1 * b2, f2;
    f2 = { |x3, y3 = 1|
        var a3 = x3 + a2 + a1, b3 = y3 * b2 * b1, c3 = c2 + c1, d3 = d2 - d1, e3, f3;
        e3 = [a3, b3, c3, d3].collect { |x4, i4|
            var a4 = x4 + a3 + a2, b4 = i4 + b3 + b2 + b1, c4 = c3 * c2, d4 = d3 + d2 + d1, e4, f4;
            f4 = { |x5|
                var a5 = x5 + a4 + a3 + a2 + a1, b5 = b4 - b3, c5 = c4 + c3 + c2 + c1, d5 = d4 * d3, e5, f5;
                e5 = (0..3).do { |x6, i6|
                    var a6 = x6 + a5 + a4, b6 = i6 + b5 + b4 + b3, c6 = c5 - c4, d6 = d5 + d4 + d3 + d2, e6;
                    e6 = { |x7, y7|
                        var a7 = x7 + a6 + a5 + a4 + a3, b7 = y7 + b6 + b5, c7 = c6 + c5 + c4 + c3 + c2;
                        var d7 = d6 + d5 + d4, e7 = 0;
                        [a7, b7, c7, d7].do { |x8, i8|
                            var a8 = x8 + a7 + a6 + a5 + a4 + a3 + a2 + a1;
                            var b8 = i8 + b7 + b6 + b5 + b4 + b3 + b2 + b1;
                            var c8 = c7 + c6 + c5 + c4 + c3 + c2 + c1;
                            var d8 = d7 + d6 + d5 + d4 + d3 + d2 + d1;
                            if (a8 > b8) { e7 = e7 + a8; f1 = f1 + 1 } { e7 = e7 - b8; e1 = e1 - 1 };
                            while { c8 > d8 } { c8 = c8 - 1; e6 = c8 };
                            a1 = a1 + (a8 + b8 + c8 + d8 % 7);
                        };
                        e7 + e5 + e4 + e3 + e2
                    }.value(a6, b6);
                    f5 = e6 + f4 + f3 + f2;
                    b5 = b5 + e6;
                };
                [a5, b5, c5, d5, e5, f5]
            };
            e4 = f4.value(x4).sum;
            a4 + b4 + c4 + d4 + e4
        };
        f3 = e3.sum + a3;
        [e3, f3]
    };
    f2.value(x2, i2)
};
)
)sc";

std::vector<Snippet> built_in_snippets() {
    return { { "one liner", one_liner },
             { "few functions", few_functions },
             { "large block", large_block },
             { "deeply nested", deeply_nested } };
}

std::optional<std::string> read_file(const std::filesystem::path& path) {
    std::ifstream file_stream { path };
    if (!file_stream.is_open())
        return std::nullopt;
    std::stringstream ss {};
    ss << file_stream.rdbuf();
    return std::move(ss).str();
}

////////////////////////////////////////////////////////////////////////////////
// Timing
////////////////////////////////////////////////////////////////////////////////

using Clock = std::chrono::steady_clock;

double seconds_since(Clock::time_point start) { return std::chrono::duration<double>(Clock::now() - start).count(); }

// In seconds. Parsing includes copying the text, as an editor's is given as a string.
struct StageTimes {
    double parse = 0;
    double sema = 0;
    double frames = 0;
    double lower = 0;
    double emit = 0;
    double finish = 0;
    double total = 0;
};

struct Compiled {
    StageTimes times;
    std::size_t num_diagnostics;
    std::size_t num_functions;
};

// Empty if it didn't get through sema.
std::optional<Compiled> compile(sc::compiler::ClassLibrary& library, const std::string& source,
                                sc::ast::LanguageVersion version) {
    namespace compiler = sc::compiler;
    StageTimes t;
    std::size_t num_diagnostics = 0;

    auto start = Clock::now();
    auto text = std::make_shared<sc::src::TextInfo>(sc::src::NormalisedSource { source }, sc::src::FileCodeLocation {},
                                                    std::nullopt, sc::src::TextKind::InterpretedFunction);
    auto parsed = compiler::parse_interpreted(std::move(text));
    t.parse = seconds_since(start);
    num_diagnostics += parsed.diagnostics.size();

    start = Clock::now();
    auto ir = compiler::compile_interpreted_ir(library, std::move(parsed), version);
    t.sema = seconds_since(start);
    num_diagnostics += ir.diagnostics.size();
    if (!ir.graph)
        return std::nullopt;
    const auto& g = *ir.graph;

    start = Clock::now();
    auto framed = compiler::layout_frames(library, g, std::get<sc::ir::SCInterpreted_I>(*g.root()));
    t.frames = seconds_since(start);
    num_diagnostics += framed.frames.diagnostics.size();

    start = Clock::now();
    auto lowered = compiler::lower(library, std::move(framed));
    t.lower = seconds_since(start);

    start = Clock::now();
    auto emitted = compiler::emit(library, std::move(lowered));
    t.emit = seconds_since(start);
    num_diagnostics += emitted.emitted.diagnostics.size();

    start = Clock::now();
    const auto compiled = compiler::finish(library, std::move(emitted));
    t.finish = seconds_since(start);

    t.total = t.parse + t.sema + t.frames + t.lower + t.emit + t.finish;
    return Compiled { t, num_diagnostics, compiled.functions.size() };
}

// Each stage's median, or fastest, over the runs.
StageTimes summarise(std::vector<StageTimes> runs, bool fastest) {
    StageTimes s;
    for (const auto stage : { &StageTimes::parse, &StageTimes::sema, &StageTimes::frames, &StageTimes::lower,
                              &StageTimes::emit, &StageTimes::finish, &StageTimes::total }) {
        const auto by_stage = [stage](const StageTimes& a, const StageTimes& b) { return a.*stage < b.*stage; };
        if (fastest) {
            s.*stage = (*std::min_element(runs.begin(), runs.end(), by_stage)).*stage;
        } else {
            const auto middle = runs.begin() + static_cast<std::ptrdiff_t>(runs.size() / 2);
            std::nth_element(runs.begin(), middle, runs.end(), by_stage);
            s.*stage = (*middle).*stage;
        }
    }
    return s;
}

////////////////////////////////////////////////////////////////////////////////
// Output
////////////////////////////////////////////////////////////////////////////////

constexpr int name_width = 16;
constexpr int column_width = 9;

void write_header(std::ostream& out) {
    out << std::left << std::setw(name_width) << "snippet" << std::right;
    for (const char* column : { "", "bytes", "funcs", "parse", "sema", "frames", "lower", "emit", "finish", "total" })
        out << std::setw(column_width) << column;
    out << "\n";
}

void write_row(std::ostream& out, const std::string& name, const char* kind, std::size_t bytes,
               std::size_t num_functions, const StageTimes& t) {
    out << std::left << std::setw(name_width) << name << std::right << std::setw(column_width) << kind
        << std::setw(column_width) << bytes << std::setw(column_width) << num_functions << std::fixed
        << std::setprecision(1);
    for (const double seconds : { t.parse, t.sema, t.frames, t.lower, t.emit, t.finish, t.total })
        out << std::setw(column_width) << seconds * 1e6;
    out << std::defaultfloat << "\n";
}

bool parse_count(const std::string& text, std::size_t& count) {
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), count);
    return error == std::errc {} && end == text.data() + text.size() && count > 0;
}
}

int main(int argc, char* argv[]) {
    sc::util::IndexedVector<sc::ir::ModuleIndex, std::filesystem::path> module_paths;
    std::vector<Snippet> snippets;
    std::size_t num_runs = 1000;
    std::size_t num_threads = std::max(1u, std::thread::hardware_concurrency());
    auto version = sc::ast::LanguageVersion::V3_16;
    for (int i { 1 }; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--runs" && i + 1 < argc) {
            if (!parse_count(argv[++i], num_runs)) {
                std::cout << "ERROR: --runs expects a positive number, not '" << argv[i] << "'" << std::endl;
                return EXIT_FAILURE;
            }
        } else if (arg == "--threads" && i + 1 < argc) {
            if (!parse_count(argv[++i], num_threads)) {
                std::cout << "ERROR: --threads expects a positive number, not '" << argv[i] << "'" << std::endl;
                return EXIT_FAILURE;
            }
        } else if (arg == "--interpreted-version" && i + 1 < argc) {
            const auto release = sc::ast::parse_release(argv[++i]);
            if (!release) {
                std::cout << "ERROR: --interpreted-version expects a known release, e.g., 3.16, or legacy, not '"
                          << argv[i] << "'" << std::endl;
                return EXIT_FAILURE;
            }
            version = *release;
        } else if (std::filesystem::path path { arg }; path.extension() == ".scd") {
            auto source = read_file(path);
            if (!source) {
                std::cout << "ERROR: could not open " << path << std::endl;
                return EXIT_FAILURE;
            }
            snippets.push_back({ path.filename().string(), std::move(*source) });
        } else {
            module_paths.push_back(std::move(path));
        }
    }
    if (module_paths.empty()) {
        std::cout << "ERROR: expected [--runs n] [--threads n] [--interpreted-version x.y] [dir|.sc file]... "
                     "[.scd file]..."
                  << std::endl;
        return EXIT_FAILURE;
    }
    if (snippets.empty())
        snippets = built_in_snippets();

    const auto library_start = Clock::now();
    sc::compiler::ClassLibraryTimes library_times;
    const auto library = sc::compiler::compile_class_library(sc::compiler::find_class_files(module_paths), num_threads,
                                                             library_times, sc::compiler::VMDefinitions {});
    const auto backend_start = Clock::now();
    const auto graphs = sc::compiler::compile_class_graphs(*library, num_threads);
    const auto backend = seconds_since(backend_start);
    const auto library_total = seconds_since(library_start);
    std::cout << std::fixed << std::setprecision(4) << "Class library, " << graphs.size() << " graphs, " << num_threads
              << " threads: parse " << library_times.parse << " s, sema " << library_times.sema << " s, member table "
              << library_times.member_table << " s, backend " << backend << " s, total " << library_total << " s"
              << std::defaultfloat << "\n\n";

    std::cout << "Interpreted, " << num_runs << " runs, in microseconds:\n";
    write_header(std::cout);
    constexpr std::size_t warm_up_runs = 10;
    for (const auto& snippet : snippets) {
        std::vector<StageTimes> runs;
        runs.reserve(num_runs);
        std::optional<Compiled> first;
        for (std::size_t run = 0; run < warm_up_runs + num_runs; ++run) {
            auto compiled = compile(*library, snippet.source, version);
            if (!compiled) {
                std::cout << "ERROR: " << snippet.name << " didn't compile, does the library have an Interpreter?"
                          << std::endl;
                return EXIT_FAILURE;
            }
            if (run >= warm_up_runs)
                runs.push_back(compiled->times);
            if (!first)
                first = compiled;
        }
        write_row(std::cout, snippet.name, "median", snippet.source.size(), first->num_functions,
                  summarise(runs, false));
        write_row(std::cout, "", "fastest", snippet.source.size(), first->num_functions, summarise(runs, true));
        if (first->num_diagnostics != 0)
            std::cout << "  (" << first->num_diagnostics << " diagnostics)\n";
    }
    return 0;
}
