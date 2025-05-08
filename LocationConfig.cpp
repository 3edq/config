#include "LocationConfig.hpp"

LocationConfig::LocationConfig()
    : autoindex(false), maxBodySize(0), returnCode(0) {
}

void LocationConfig::setPath(const std::string& p) {
    path = p;
}

void LocationConfig::setRoot(const std::string& r) {
    root = r;
}

void LocationConfig::setIndex(const std::string& i) {
    index = i;
}

void LocationConfig::setAutoindex(bool a) {
    autoindex = a;
}

void LocationConfig::setCgiPath(const std::string& p) {
    cgiPath = p;
}

void LocationConfig::setUploadPath(const std::string& p) {
    uploadPath = p;
}

void LocationConfig::setMaxBodySize(int size) {
    maxBodySize = size;
}

void LocationConfig::setReturnCode(int code) {
    returnCode = code;
}

void LocationConfig::setRedirectPath(const std::string& path) {
    redirectPath = path;
}

void LocationConfig::setMethods(const std::vector<std::string>& m) {
    methods = m;
}

const std::string& LocationConfig::getPath() const {
    return path;
}

const std::string& LocationConfig::getRoot() const {
    return root;
}

const std::string& LocationConfig::getIndex() const {
    return index;
}

bool LocationConfig::getAutoindex() const {
    return autoindex;
}

const std::string& LocationConfig::getCgiPath() const {
    return cgiPath;
}

const std::string& LocationConfig::getUploadPath() const {
    return uploadPath;
}

int LocationConfig::getMaxBodySize() const {
    return maxBodySize;
}

int LocationConfig::getReturnCode() const {
    return returnCode;
}

const std::string& LocationConfig::getRedirectPath() const {
    return redirectPath;
}

const std::vector<std::string>& LocationConfig::getMethods() const {
    return methods;
}
