#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "LocationConfig.hpp"
#include <string>
#include <vector>
#include <map>

class ServerConfig {
public:
    ServerConfig();

    void setPort(int port);
    void setServerName(const std::string& name);
    void setRoot(const std::string& path);
    void addErrorPage(int code, const std::string& path);
    void addLocation(const LocationConfig& loc);

    int getPort() const;
    const std::string& getServerName() const;
    const std::string& getRoot() const;
    const std::map<int, std::string>& getErrorPages() const;
    const std::vector<LocationConfig>& getLocations() const;

private:
    int port;
    std::string serverName;
    std::string root;
    std::map<int, std::string> errorPages;
    std::vector<LocationConfig> locations;
};

#endif
