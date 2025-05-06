#include "ConfigParser.hpp"

int main() {
    ConfigParser parser;

    try {
        parser.Parse("example.conf");
        const std::vector<ServerBlock>& servers = parser.getParsedServers();

        for (size_t i = 0; i < servers.size(); ++i) {
            std::cout << "--- Server Block #" << i << " ---" << std::endl;
            for (size_t j = 0; j < servers[i].configTokens.size(); ++j) {
                const Token& t = servers[i].configTokens[j];
                std::cout << "[ConfigToken] " << t.line << ": " << t.value << std::endl;
            }

            for (size_t k = 0; k < servers[i].locations.size(); ++k) {
                std::cout << "  Location path: " << servers[i].locations[k].path << std::endl;
                for (size_t l = 0; l < servers[i].locations[k].tokens.size(); ++l) {
                    const Token& t = servers[i].locations[k].tokens[l];
                    std::cout << "    [LocationToken] " << t.line << ": " << t.value << std::endl;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
