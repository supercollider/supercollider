#include "boost/program_options.hpp"
#include <iostream>
#include <string>
#include "SC_TerminalClient.h"
#include "SC_CLIOptions.hpp"

#include <SC_Filesystem.hpp>
#include <SC_LanguageConfig.hpp>
#include <SC_Version.hpp>

namespace po = boost::program_options;

namespace SC_CLI {

/// parses the CLI - if the program should terminate after parsing it will return an exit code.
/// If parsing was successful and the program should continue it will return a std::nullopt .
std::optional<int> CLIOptions::parse(int argc, char* argv[], SC_TerminalClient::Options& terminalOptions) {
    po::options_description description("Interpreter for sclang (SuperCollider language)");

    auto genericDescription = buildGenericDescription();
    auto terminalDescription = buildTerminalDescription(terminalOptions);
    auto languageConfigDescription = buildLanguageConfigDescription();

    // Hidden options, will be allowed both on command line and
    // in config file, but will not be shown to the user.
    po::options_description hiddenDescription("Hidden options");
    // clang-format off
    hiddenDescription.add_options()
        ("input-file", po::value<std::string>(&mInputFile), "input file")
        ("sc-args", po::value<std::vector<std::string>>(&mScArgs), "sc args")
    ;
    // clang-format on

    // positional options are declared here but captured via hidden description
    po::positional_options_description positionalOptions;
    positionalOptions.add("input-file", 1);
    positionalOptions.add("sc-args", -1);

    po::options_description cmdLineOptions;
    cmdLineOptions.add(genericDescription)
        .add(terminalDescription)
        .add(hiddenDescription)
        .add(languageConfigDescription);

    po::variables_map vm;
    try {
        store(po::command_line_parser(argc, argv).options(cmdLineOptions).positional(positionalOptions).run(), vm);
        notify(vm);
    } catch (po::unknown_option& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (po::invalid_option_value& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    if (mParsingFailed) {
        return 1;
    }

    if (vm.count("sc-args")) {
        terminalOptions.mArgs = vm["sc-args"].as<std::vector<std::string>>();
    }

    if (mPrintVersion) {
        std::cout << "sclang " << SC_VersionString() << " (" << SC_BuildString() << ")" << std::endl;
        return 0;
    }

    // this is handled here and not in the description declaration b/c we need access to the declarations
    if (mPrintHelp) {
        // construct a options_description w/o hidden positional options
        po::options_description visibleDescription;
        visibleDescription.add(genericDescription).add(terminalDescription).add(languageConfigDescription);

        std::cout
            << "Start a REPL interpreter:           sclang [ -options ]\n"
               "Execute a .scd file:                sclang [ -options ] path_to_sc_file [ args passed to sclang... ]\n"
            << visibleDescription << std::endl;
        return 0;
    }

    if (!mInputFile.empty()) {
        terminalOptions.mDaemon = true;
    }

    SC_LanguageConfig::readLibraryConfig(terminalOptions.mStandalone);

    // for the language config we don't use the notifier of program options because
    // we want to fill the config with a provided yaml file or their default values.
    // as po does not allow to specify the "notify order", we parse the config manually
    parseLanguageConfig(vm);

    return std::nullopt;
}

po::options_description CLIOptions::buildGenericDescription() {
    po::options_description options("Generic options");
    // clang-format off
    options.add_options()
        ("help,h", po::bool_switch(&mPrintHelp), "Print this message and exit")
        ("version,v", po::bool_switch(&mPrintVersion), "Print sclang version and exit")
    ;
    // clang-format on
    return options;
}

po::options_description CLIOptions::buildTerminalDescription(SC_TerminalClient::Options& terminalOptions) {
    po::options_description description("sclang options");

    // clang-format off
    description.add_options()
        (
            "runtime-directory,d",
            po::value<std::string>(&terminalOptions.mRuntimeDir),
            "Set runtime directory"
        )
        (
            "daemon,D",
            po::bool_switch(&terminalOptions.mDaemon)->default_value(terminalOptions.mDaemon),
            "Enter daemon mode"
        )
        (
            "heap-growth,g",
            po::value<std::string>()->default_value(convertMemoryInteger(terminalOptions.mMemGrow))->notifier([&](const std::string &heapGrowth) {
                terminalOptions.mMemGrow = parseMemoryString(heapGrowth);
            }),
            "Set heap growth size (allows k, m and g as suffix multipliers)"
        )
        (
            "yaml-config,l",
            po::value<std::string>(&terminalOptions.mLibraryConfigFile)->notifier([&](const std::string &yaml_config) {
                terminalOptions.mLibraryConfigFile = yaml_config;
                SC_LanguageConfig::setConfigPath(yaml_config);
            }),
            "Set yaml config"
        )
        (
            "heap-size,m",
            po::value<std::string>()->default_value(convertMemoryInteger(terminalOptions.mMemSpace))->notifier([&](const std::string &heapSize) {
                terminalOptions.mMemSpace = parseMemoryString(heapSize);
            }),
            "Set initial heap size (allows k, m and g as suffix multipliers)"
        )
        (
            "call-run,r",
            po::bool_switch(&terminalOptions.mCallRun)->default_value(terminalOptions.mCallRun),
            "Call Main.run on startup"
        )
        (
            "call-stop,s",
            po::bool_switch(&terminalOptions.mCallStop)->default_value(terminalOptions.mCallStop),
            "Call Main.stop on shutdown"
        )
        (
            "port,u",
            po::value<int>(&terminalOptions.mPort)->default_value(terminalOptions.mPort)->notifier([&](int port) {
                if (port < 0 || port > 65535) {
                    std::cerr << "UDP port must be in range between 0-65535, but was " << port << std::endl;
                    mParsingFailed = true;
                }
            }),
            "Set UDP listening port"
        )
        (
            "ide,i",
            po::value<std::string>()->notifier([&](std::string ide_name) {
                SC_Filesystem::instance().setIdeName(ide_name);
            }),
            "Set IDE name"
        )
        (
            "exclude-default-paths,a",
            po::bool_switch(&terminalOptions.mStandalone)->default_value(terminalOptions.mStandalone),
            "Exclude searching in default SCClassLibrary and user and system extensions paths (aka 'standalone' mode)."
        )
    ;
    // clang-format on

    return description;
}

po::options_description CLIOptions::buildLanguageConfigDescription() {
    po::options_description description("Language config options (overwrites provided yaml entries)");

    // clang-format off
    description.add_options()
        (
            "include-path",
            po::value<std::vector<std::string>>(),
            "Class library path to be included for searching (allows multiple mentions)"
        )
        (
            "exclude-path",
            po::value<std::vector<std::string>>(),
            "Class library path to be excluded from searching (overrides includePaths, allows multiple mentions)"
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

void CLIOptions::parseLanguageConfig(po::variables_map& vm) {
    // see HelpSource/Classes/LanguageConfig.schelp
    if (vm.at("post-inline-warnings").as<bool>()) {
        gLanguageConfig->setPostInlineWarnings(true);
    };

    if (vm.count("include-path")) {
        for (auto includePath : vm["include-path"].as<std::vector<std::string>>()) {
            gLanguageConfig->addIncludedDirectory(SC_Codecvt::utf8_str_to_path(includePath));
        }
    }
    if (vm.count("exclude-path")) {
        for (auto excludePath : vm["exclude-path"].as<std::vector<std::string>>()) {
            gLanguageConfig->addExcludedDirectory(SC_Codecvt::utf8_str_to_path(excludePath));
        }
    }
}

// converts e.g. 1k to 1024
int CLIOptions::parseMemoryString(const std::string& input) {
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
        std::cerr << "Couldn't parse memory value '" << input << "': '" << suffix << "' is an unknown modifier"
                  << std::endl;
        exit(1);
    }

    std::string numberPart = std::isdigit(suffix) ? input : input.substr(0, input.size() - 1);
    try {
        int value = boost::lexical_cast<int>(numberPart);
        return value * multiplier;
    } catch (const boost::bad_lexical_cast&) {
        std::cerr << "Couldn't parse memory value '" << input << "'" << std::endl;
        exit(1);
    }
}

// converts e.g. 1024 to 1k
std::string CLIOptions::convertMemoryInteger(int memSize) {
    int rem = memSize;
    int mod = 0;
    std::string modChar;

    while ((rem % 1024) == 0 && (mod < 4)) {
        rem /= 1024;
        mod++;
    }

    switch (mod) {
    case 1:
        modChar = 'k';
        break;
    case 2:
        modChar = 'm';
        break;
    case 3:
        modChar = 'g';
        break;
    default:
        rem = memSize;
        modChar = "";
    }

    return std::to_string(rem) + modChar;
}
}
