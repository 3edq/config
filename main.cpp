#include "ConfigParser.hpp"
#include <iostream>

int main() {
    ConfigParser parser;

    try {
        parser.Parse("example.conf");
        const std::vector<ServerConfig>& servers = parser.getServers();

        ServerConfig::validateAll(servers);

        for (size_t i = 0; i < servers.size(); ++i) {
            std::cout << "--- Server #" << i << " ---\n";
            std::cout << "Port: " << servers[i].getPort() << "\n";
            std::cout << "ServerName: " << servers[i].getServerName() << "\n";
            const std::map<int, std::string>& errorPages = servers[i].getErrorPages();
            if (!errorPages.empty()) {
                std::cout << "Error Pages:\n";
                for (std::map<int, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it) {
                    std::cout << "  " << it->first << ": " << it->second << "\n";
                }
            }
            const std::vector<LocationConfig>& locs = servers[i].getLocations();
            for (size_t j = 0; j < locs.size(); ++j) {
                std::cout << "  [Location] Path: " << locs[j].getPath() << "\n";
                std::cout << "    Root: " << locs[j].getRoot() << "\n";
                std::cout << "    Index: " << locs[j].getIndex() << "\n";
                std::cout << "    Autoindex: " << (locs[j].getAutoindex() ? "on" : "off") << "\n";
                std::cout << "    CGI Path: " << locs[j].getCgiPath() << "\n";
                std::cout << "    MaxBodySize: " << locs[j].getMaxBodySize() << "\n";
                std::cout << "    Return: " << locs[j].getReturnCode() << " " << locs[j].getRedirectPath() << "\n";
                std::cout << "    Methods: ";
                for (size_t m = 0; m < locs[j].getMethods().size(); ++m)
                    std::cout << locs[j].getMethods()[m] << " ";
                std::cout << "\n";
            }
        }
    } catch (std::exception& e) {
        std::cerr << "Config Error: " << e.what() << "\n";
    }

    return 0;
}
