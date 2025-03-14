#include "boost/program_options.hpp"
#include <iostream>
#include <string>
#include "SC_TerminalClient.h"
#include "SC_CLIOptions.hpp"

#include <SC_Filesystem.hpp>
#include <SC_LanguageConfig.hpp>
#include <SC_Version.hpp>

namespace po = boost::program_options;
using namespace std;

namespace SC_CLI {

/// parses the CLI - if the program should terminate after parsing it will return true
/// an exit code is passed by reference
bool CLIOptions::parse(int argc, char* argv[], SC_TerminalClient::Options& terminal_options, int& exit_code) {
    exit_code = 0;
    po::options_description description("Interpreter for sclang (SuperCollider language)");

    auto generic_description = build_generic_description();
    auto terminal_description = build_terminal_description(terminal_options);
    auto language_config_description = build_language_config_description();

    // Hidden options, will be allowed both on command line and
    // in config file, but will not be shown to the user.
    po::options_description hidden_description("Hidden options");
    hidden_description.add_options()("input-file", po::value<string>(&mInputFile),
                                     "input file")("sc-args", po::value<vector<string>>(&mScArgs), "sc args");

    // positional options are declared here but captured via hidden description
    po::positional_options_description pos_options;
    pos_options.add("input-file", 1);
    pos_options.add("sc-args", -1);

    po::options_description cmdline_options;
    cmdline_options.add(generic_description)
        .add(terminal_description)
        .add(hidden_description)
        .add(language_config_description);

    po::variables_map vm;
    try {
        store(po::command_line_parser(argc, argv).options(cmdline_options).positional(pos_options).run(), vm);
        notify(vm);
    } catch (po::unknown_option& e) {
        cerr << e.what() << endl;
        exit_code = 1;
        return true;
    } catch (po::invalid_option_value& e) {
        cerr << e.what() << endl;
        exit_code = 1;
        return true;
    }
    if (mParsingFailed) {
        exit_code = 1;
        return true;
    }

    if (vm.count("sc-args")) {
        terminal_options.mArgs = vm["sc-args"].as<vector<string>>();
    }

    if (mPrintVersion) {
        cout << "sclang " << SC_VersionString() << " (" << SC_BuildString() << ")" << endl;
        return true;
    }

    // this is handled here and not in the description declaration b/c we need access to the declarations
    if (mPrintHelp) {
        // construct a options_description w/o hidden positional options
        po::options_description visible_description("");
        visible_description.add(generic_description).add(terminal_description).add(language_config_description);

        cout << "Start a REPL interpreter:           sclang [ -options ]\n"
                "Execute a .scd file:                sclang [ -options ] path_to_sc_file [ args passed to sclang... ]\n"
             << visible_description << endl;
        return true;
    }

    if (!mInputFile.empty()) {
        terminal_options.mDaemon = true;
    }

    SC_LanguageConfig::readLibraryConfig(terminal_options.mStandalone);

    // for the language config we don't use the notifier of program options because
    // we want to fill the config with a provided yaml file or their default values.
    // as po does not allow to specify the "notify order", we parse the config manually
    parse_language_config(vm);


    return false;
}

po::options_description CLIOptions::build_generic_description() {
    po::options_description options("Generic options");
    options.add_options()("help,h", po::bool_switch(&mPrintHelp), "Print this message and exit")(
        "version,v", po::bool_switch(&mPrintVersion), "Print sclang version and exit");
    return options;
}

po::options_description CLIOptions::build_terminal_description(SC_TerminalClient::Options& terminal_options) {
    po::options_description description("sclang options");

    // clang-format off
    description.add_options()
        (
            "runtime-directory,d",
            po::value<string>(&terminal_options.mRuntimeDir),
            "Set runtime directory"
        )
        (
            "daemon,D",
            po::bool_switch(&terminal_options.mDaemon)->default_value(terminal_options.mDaemon),
            "Enter daemon mode"
        )
        (
            "heap-growth,g",
            po::value<string>()->default_value(convert_memory_integer(terminal_options.mMemGrow))->notifier([&](const string &heapGrowth) {
                terminal_options.mMemGrow = parse_memory_string(heapGrowth);
            }),
            "Set heap growth size (allows k, m and g as suffix multipliers)"
        )
        (
            "yaml-config,l",
            po::value<string>(&terminal_options.mLibraryConfigFile)->notifier([&](const string &yaml_config) {
                terminal_options.mLibraryConfigFile = yaml_config;
                SC_LanguageConfig::setConfigPath(yaml_config);
            }),
            "Set yaml config"
        )
        (
            "heap-size,m",
            po::value<string>()->default_value(convert_memory_integer(terminal_options.mMemSpace))->notifier([&](const string &heapSize) {
                terminal_options.mMemSpace = parse_memory_string(heapSize);
            }),
            "Set initial heap size (allows k, m and g as suffix multipliers)"
        )
        (
            "call-run,r",
            po::bool_switch(&terminal_options.mCallRun)->default_value(terminal_options.mCallRun),
            "Call Main.run on startup"
        )
        (
            "call-stop,s",
            po::bool_switch(&terminal_options.mCallStop)->default_value(terminal_options.mCallStop),
            "Call Main.stop on shutdown"
        )
        (
            "port,u",
            po::value<int>(&terminal_options.mPort)->default_value(terminal_options.mPort)->notifier([&](int port) {
                if (port < 0 || port > 65535) {
                    cerr << "UDP port must be in range between 0-65535, but was " << port << endl;
                    mParsingFailed = true;
                }
            }),
            "Set UDP listening port"
        )
        (
            "ide,i",
            po::value<string>()->notifier([&](string ide_name) {
                SC_Filesystem::instance().setIdeName(ide_name);
            }),
            "Set IDE name"
        )
        (
            "standalone,a",
            po::bool_switch(&terminal_options.mStandalone)->default_value(terminal_options.mStandalone),
            "Standalone mode (exclude SCClassLibrary and user and system Extensions folders from search path)"
        )
    ;
    // clang-format on

    return description;
}

po::options_description CLIOptions::build_language_config_description() {
    po::options_description description("Language config options (overwrites provided yaml entries)");

    // clang-format off
    description.add_options()
        (
            "include-path",
            po::value<vector<string>>(),
            "class library path to be included for searching (allows multiple mentions)"
        )
        (
            "exclude-path",
            po::value<vector<string>>(),
            "class library path to be to excluded from searching (overrides includePaths, allows multiple mentions)"
        )
        (
            "post-inline-warnings",
            po::bool_switch()->default_value(false),
            "Boolean flag to turn on warnings about missing inline opportunities."
        )
        // exclude-default-paths is not included as this behavior is set via the `--standalone` flag
    ;
    // clang-format on
    return description;
}

void CLIOptions::parse_language_config(po::variables_map& vm) {
    // see HelpSource/Classes/LanguageConfig.schelp
    if (vm.at("post-inline-warnings").as<bool>()) {
        gLanguageConfig->setPostInlineWarnings(true);
    };

    if (vm.count("include-path")) {
        for (auto includePath : vm["include-path"].as<vector<string>>()) {
            gLanguageConfig->addIncludedDirectory(SC_Codecvt::utf8_str_to_path(includePath));
        }
    }
    if (vm.count("exclude-path")) {
        for (auto excludePath : vm["exclude-path"].as<vector<string>>()) {
            gLanguageConfig->addExcludedDirectory(SC_Codecvt::utf8_str_to_path(excludePath));
        }
    }
}

// converts e.g. 1k to 1024
int CLIOptions::parse_memory_string(std::string input) {
    if (input.empty()) {
        return 0;
    }

    char suffix = input.back();
    int multiplier = 1;

    if (suffix == 'k' || suffix == 'K') {
        multiplier = 1024;
    } else if (suffix == 'm' || suffix == 'M') {
        multiplier = 1024 * 1024;
    } else if (suffix == 'g' || suffix == 'G') {
        multiplier = 1024 * 1024 * 1024;
    } else if (!isdigit(suffix)) {
        cerr << "Unknown modifier '" << suffix << "'" << endl;
        return 0;
    }

    std::string number_part = std::isdigit(suffix) ? input : input.substr(0, input.size() - 1);
    int value = boost::lexical_cast<int>(number_part);

    return value * multiplier;
}

// converts e.g. 1024 to 1k
std::string CLIOptions::convert_memory_integer(int mem_size) {
    int rem = mem_size;
    int mod = 0;
    std::string mod_char;

    while ((rem % 1024) == 0 && (mod < 4)) {
        rem /= 1024;
        mod++;
    }

    switch (mod) {
    case 1:
        mod_char = 'k';
        break;
    case 2:
        mod_char = 'm';
        break;
    case 3:
        mod_char = 'g';
        break;
    default:
        rem = mem_size;
        mod_char = "";
    }

    return std::to_string(rem) + mod_char;
}
}
