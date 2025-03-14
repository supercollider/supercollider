#include "boost/program_options.hpp"
#include <string>
#include "SC_TerminalClient.h"

namespace po = boost::program_options;
using namespace std;

namespace SC_CLI {
class CLIOptions {
public:
    CLIOptions() {};
    bool parse(int argc, char* argv[], SC_TerminalClient::Options& terminal_options, int& exit_code);

    string mInputFile = "";
    vector<string> mScArgs = {};

private:
    bool mParsingFailed = false;
    bool mPrintHelp = false;
    bool mPrintVersion = false;

    po::options_description build_generic_description();
    po::options_description build_terminal_description(SC_TerminalClient::Options& terminal_options);
    static po::options_description build_language_config_description();

    static void parse_language_config(po::variables_map& vm);

    static int parse_memory_string(std::string input);
    static std::string convert_memory_integer(int mem_size);
};
}
