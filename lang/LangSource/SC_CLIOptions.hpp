#include "boost/program_options.hpp"
#include <string>
#include <optional>
#include "SC_TerminalClient.h"

namespace po = boost::program_options;

namespace SC_CLI {
class CLIOptions {
public:
    std::optional<int> parse(int argc, char* argv[], SC_TerminalClient::Options& terminalOptions);

    std::string mInputFile;
    std::vector<std::string> mScArgs;

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
