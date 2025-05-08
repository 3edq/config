#include "ServerConfig.hpp"

ServerConfig::ServerConfig() : port(0) {}

void ServerConfig::setPort(int p) {
    port = p;
}

void ServerConfig::setServerName(const std::string& name) {
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
