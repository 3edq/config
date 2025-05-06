#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cctype>
#include <stdexcept>

enum TokenType {
    KEY, VALUE, BRACE_OPEN, BRACE_CLOSE, SEMICOLON
};

struct Token {
    TokenType type;
    std::string value;
    int line;
};

class ConfigParser{
    public:
    ConfigParser();
    ~ConfigParser();
    void Parse(const std::string& filepath);
    std::vector<Token> tokenize(const std::string& content);
    std::string readfile(const std::string& path);
    std::string removeComment(const std::string& content);
    std::string trimWhitespace(const std::string& content);

    private:

};

#endif