#include"ConfigParser.hpp"

ConfigParser::ConfigParser() {}

ConfigParser::~ConfigParser() {}

std::string ConfigParser::readfile(const std::string& path){
    std::ifstream file(path.c_str());
    if(!file.is_open())
        throw std::runtime_error("Failed to open file: " + path);
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::string ConfigParser::removeComment(const std::string& content){
    std::istringstream stream(content);
    std::ostringstream result;
    std::string line;

    while(std::getline(stream,line)){
        size_t pos = line.find('#');
        if(pos!=std::string::npos){
            line = line.substr(0,pos);
        }
        result << line << '\n';
    }
    return result.str();
}

std::string ConfigParser::trimWhitespace(const std::string& content){
    std::istringstream stream(content);
    std::ostringstream result;
    std::string line;
    while(std::getline(stream,line)){
        size_t start = line.find_first_not_of(" \t\r");
        size_t end = line.find_last_not_of(" \t\r");
        if (start != std::string::npos && end != std::string::npos)
        result << line.substr(start, end - start + 1) << '\n';
    }
    return result.str();
}

std::vector<Token> ConfigParser::tokenize(const std::string& content) {
    std::vector<Token> tokens;
    std::istringstream stream(content);
    std::string line;
    int lineNum = 1;

    while (std::getline(stream, line)) {
        std::istringstream linestream(line);
        std::string word;
        while (linestream >> word) {
            Token t;
            if (word == "{") {
                t.type = BRACE_OPEN;
                t.value = word;
                t.line = lineNum;
                tokens.push_back(t);
            } else if (word == "}") {
                t.type = BRACE_CLOSE;
                t.value = word;
                t.line = lineNum;
                tokens.push_back(t);
            } else if (!word.empty() && word[word.size() - 1] == ';') {
                std::string valuePart = word.substr(0, word.size() - 1);
                if (!valuePart.empty()) {
                    t.type = VALUE;
                    t.value = valuePart;
                    t.line = lineNum;
                    tokens.push_back(t);
                }
                t.type = SEMICOLON;
                t.value = ";";
                t.line = lineNum;
                tokens.push_back(t);
            } else {
                if (tokens.empty() || tokens[tokens.size() - 1].type == SEMICOLON || tokens[tokens.size() - 1].type == BRACE_OPEN)
                    t.type = KEY;
                else
                    t.type = VALUE;
                t.value = word;
                t.line = lineNum;
                tokens.push_back(t);
            }
        }
        ++lineNum;
    }
    return tokens;
}

void ConfigParser::Parse(const std::string& filepath){
    std::string raw = readfile(filepath);
    std::string noComment = removeComment(raw);
    std::string cleaned = trimWhitespace(noComment);
    std::vector<Token> tokens = tokenize(cleaned);

    for(size_t i = 0;i < tokens.size();i++){
        std::cout << tokens[i].line << ": ";
        switch(tokens[i].type){
            case KEY : std::cout << "KEY"; break;
            case VALUE : std::cout << "VALUE";break;
            case BRACE_OPEN : std::cout <<"{";break;
            case BRACE_CLOSE : std::cout << "}";break;
            case SEMICOLON : std::cout << ";";break;
        }
        std::cout << "=>" << tokens[i].value << std::endl;
    }
}