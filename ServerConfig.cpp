#include "ServerConfig.hpp"

ServerConfig::ServerConfig() : port(0) {}

void ServerConfig::validateAll(const std::vector<ServerConfig>& servers) {
    std::set<std::pair<int, std::string> > portNameSet;

    for (size_t i = 0; i < servers.size(); ++i) {
        const ServerConfig& s = servers[i];
        std::pair<int, std::string> key = std::make_pair(s.getPort(), s.getServerName());

        if (!portNameSet.insert(key).second) {
            std::ostringstream oss;
            oss << "Duplicate server_name + port: " << s.getServerName() << ":" << s.getPort();
            throw std::runtime_error(oss.str());
        }
    }
}

void ServerConfig::setPort(int p) {
    if (p < 1 || p > 65535) {
        std::ostringstream oss;
        oss << "Invalid listen port: " << p;
        throw std::runtime_error(oss.str());
    }
    port = p;
}

void ServerConfig::setServerName(const std::string& name) {
    if (name.empty())
        throw std::runtime_error("Server name is empty");
    for (size_t i = 0; i < name.size(); ++i) {
        if (std::isspace(name[i])) {
            std::ostringstream oss;
            oss << "Server name contains whitespace: " << name;
            throw std::runtime_error(oss.str());
        }
    }
    serverName = name;
}

void ServerConfig::setRoot(const std::string& path) {
    root = path;
}

void ServerConfig::addErrorPage(int code, const std::string& path) {
    errorPages[code] = path;
}

void ServerConfig::addLocation(const LocationConfig& loc) {
    locations.push_back(loc);
}

int ServerConfig::getPort() const {
    return port;
}

const std::string& ServerConfig::getServerName() const {
    return serverName;
}

const std::string& ServerConfig::getRoot() const {
    return root;
}

const std::map<int, std::string>& ServerConfig::getErrorPages() const {
    return errorPages;
}

const std::vector<LocationConfig>& ServerConfig::getLocations() const {
    return locations;
}
