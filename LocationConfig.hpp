#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include <string>
#include <vector>

class LocationConfig {
public:
    LocationConfig();

    void setPath(const std::string& path);
    void setRoot(const std::string& root);
    void setIndex(const std::string& index);
    void setAutoindex(bool autoindex);
    void setCgiPath(const std::string& path);
    void setUploadPath(const std::string& path);
    void setMaxBodySize(int size);
    void setReturnCode(int code);
    void setRedirectPath(const std::string& path);
    void setMethods(const std::vector<std::string>& methods);

    const std::string& getPath() const;
    const std::string& getRoot() const;
    const std::string& getIndex() const;
    bool getAutoindex() const;
    const std::string& getCgiPath() const;
    const std::string& getUploadPath() const;
    int getMaxBodySize() const;
    int getReturnCode() const;
    const std::string& getRedirectPath() const;
    const std::vector<std::string>& getMethods() const;

private:
    std::string path;
    std::string root;
    std::string index;
    bool autoindex;
    std::string cgiPath;
    std::string uploadPath;
    int maxBodySize;
    int returnCode;
    std::string redirectPath;
    std::vector<std::string> methods;
};

#endif
