#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cctype>
#include <stdexcept>
#include <cstdlib>

enum TokenType {
    KEY, VALUE, BRACE_OPEN, BRACE_CLOSE, SEMICOLON
};

struct Token {
    TokenType type;
    std::string value;
};

struct LocationBlock {
    std::string path;
    std::vector<Token> tokens;
};

struct ServerBlock{
    std::vector<Token> configTokens;
    std::vector<LocationBlock> locations;
};

class ConfigParser{
    public:
    ConfigParser();
    ~ConfigParser();
    void Parse(const std::string& filepath);
    std::vector<std::vector<Token> > extractServerBlock(const std::vector<Token>& tokens);
    std::vector<LocationBlock> extractLocationBlock(const std::vector<Token>& serverBlock);
    std::vector<Token> filterNonLocationToken(const std::vector<Token>& serverBlock);
    std::vector<Token> tokenize(const std::string& content);
    std::string readfile(const std::string& path);
    std::string removeComment(const std::string& content);
    std::string trimWhitespace(const std::string& content);
    const std::vector<ServerBlock>& getParsedServers() const;
    ServerConfig saveServerConfig(const std::vector<Token>& tokens);
    LocationConfig saveLocationConfig(const LocationBlock& lb);
    const std::vector<ServerConfig>& getServers() const;

    private:
    std::vector<ServerConfig> servers;
    std::vector<ServerBlock> parsedServers;

};

#endif