#include "boost/program_options.hpp"
#include <string>
#include "SC_TerminalClient.h"

namespace po = boost::program_options;
using namespace std;

namespace SC_CLI {
class CLIOptions {
public:
    bool parse(int argc, char* argv[], SC_TerminalClient::Options& terminalOptions, int& exitCode);

    string mInputFile;
    vector<string> mScArgs;

private:
    bool mParsingFailed = false;
    bool mPrintHelp = false;
    bool mPrintVersion = false;

    po::options_description buildGenericDescription();
    po::options_description buildTerminalDescription(SC_TerminalClient::Options& terminalOptions);
    static po::options_description buildLanguageConfigDescription();

    static void parseLanguageConfig(po::variables_map& vm);

    static int parseMemoryString(const std::string& input);
    static std::string convertMemoryInteger(int memSize);
};
}
