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

    string mInputFile;
    vector<string> mScArgs;

private:
    bool mParsingFailed = false;
    bool mPrintHelp = false;
    bool mPrintVersion = false;

    po::options_description buildGenericDescription();
    po::options_description buildTerminalDescription(SC_TerminalClient::Options& terminal_options);
    static po::options_description buildLanguageConfigDescription();

    static void parseLanguageConfig(po::variables_map& vm);

    static int parseMemoryString(std::string input);
    static std::string convertMemoryInteger(int mem_size);
};
}
