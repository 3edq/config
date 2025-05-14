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

    while (std::getline(stream, line)) {
        std::istringstream linestream(line);
        std::string word;
        while (linestream >> word) {
            Token t;
            if (word == "{") {
                t.type = BRACE_OPEN;
                t.value = word;
                tokens.push_back(t);
            } else if (word == "}") {
                t.type = BRACE_CLOSE;
                t.value = word;
                tokens.push_back(t);
            } else if (!word.empty() && word[word.size() - 1] == ';') {
                std::string valuePart = word.substr(0, word.size() - 1);
                if (!valuePart.empty()) {
                    t.type = VALUE;
                    t.value = valuePart;
                    tokens.push_back(t);
                }
                t.type = SEMICOLON;
                t.value = ";";
                tokens.push_back(t);
            } else {
                if (tokens.empty()
                || tokens[tokens.size()-1].type == SEMICOLON
                || tokens[tokens.size()-1].type == BRACE_OPEN
                || tokens[tokens.size()-1].type == BRACE_CLOSE)
                {
                    t.type = KEY;
                }
                else
                    t.type = VALUE;
                t.value = word;
                tokens.push_back(t);
            }
        }
    }
    return tokens;
}

std::vector<std::vector<Token> > ConfigParser::extractServerBlock(const std::vector<Token>& tokens){
    std::vector<std::vector<Token> > blocks;
    std::vector<Token> currentBlock;
    bool insideServer = false;
    int braceCount = 0;

    for (size_t i = 0; i < tokens.size(); ++i) {
        const Token& tok = tokens[i];

        if (!insideServer) {
            if (tok.type == KEY && tok.value == "server") {
                insideServer = true;
                currentBlock.clear();
                currentBlock.push_back(tok);
            }
        } else {
            currentBlock.push_back(tok);
            if (tok.type == BRACE_OPEN) braceCount++;
            if (tok.type == BRACE_CLOSE) braceCount--;

            if (braceCount == 0 && tok.type == BRACE_CLOSE) {
                blocks.push_back(currentBlock);
                currentBlock.clear();
                insideServer = false;
            }
        }
    }
    return blocks;
}

std::vector<LocationBlock> ConfigParser::extractLocationBlock(const std::vector<Token>& serverBlock) {
    std::vector<LocationBlock> locationBlocks;
    int braceSize = 0;
    bool insideLocation = false;
    std::string path;
    std::vector<Token> currentBlock;

    for (size_t i = 0; i < serverBlock.size(); ++i) {
        const Token& tok = serverBlock[i];
        if (!insideLocation && tok.type == KEY && tok.value == "location") {
            if (i + 2 < serverBlock.size() &&
                serverBlock[i + 1].type == VALUE &&
                serverBlock[i + 2].type == BRACE_OPEN) {
                path = serverBlock[i + 1].value;
                insideLocation = true;
                braceSize = 1;
                currentBlock.clear();
                i += 2;
                continue;
            } else {
                throw std::runtime_error("Missing Location block");
            }
        }
        if (insideLocation) {
            currentBlock.push_back(tok);
            if (tok.type == BRACE_OPEN) ++braceSize;
            if (tok.type == BRACE_CLOSE) --braceSize;

            if (braceSize == 0) {
                LocationBlock lb;
                lb.path = path;
                lb.tokens = currentBlock;
                locationBlocks.push_back(lb);
                insideLocation = false;
                currentBlock.clear();
            }
            continue;
        }
    }
    return locationBlocks;
}

std::vector<Token> ConfigParser::filterNonLocationToken(const std::vector<Token>& serverBlock) {
    std::vector<Token> result;
    bool insideLocation = false;
    int braceDepth = 0;

    for (size_t i = 0; i < serverBlock.size(); ++i) {
        const Token& tok = serverBlock[i];

        if (!insideLocation && tok.type == KEY && tok.value == "location") {
            if (i + 2 < serverBlock.size()
                && serverBlock[i + 1].type == VALUE
                && serverBlock[i + 2].type == BRACE_OPEN) {
                i += 2;
                braceDepth = 1;
                insideLocation = true;
                continue;
            } else {
                throw std::runtime_error("Invalid location block syntax");
            }
        }
        if (insideLocation) {
            if (tok.type == BRACE_OPEN)  ++braceDepth;
            if (tok.type == BRACE_CLOSE) --braceDepth;

            if (braceDepth == 0) {
                insideLocation = false;
                result.push_back(tok);
                continue;
            }
            continue;
        }
        result.push_back(tok);
    }
    return result;
}

void ConfigParser::Parse(const std::string& filepath) {
    std::string raw = readfile(filepath);
    std::string noComment = removeComment(raw);
    std::string cleaned = trimWhitespace(noComment);
    std::vector<Token> tokens = tokenize(cleaned);
    std::vector<std::vector<Token> > serverBlocks = extractServerBlock(tokens);

    this->servers.clear();

    for (size_t i = 0; i < serverBlocks.size(); i++) {
        const std::vector<Token>& serverBlock = serverBlocks[i];
        std::vector<Token> serverConfigTokens = filterNonLocationToken(serverBlock);
        std::vector<LocationBlock> locationBlocks = extractLocationBlock(serverBlock);

        ServerConfig config = saveServerConfig(serverConfigTokens);

        for (size_t j = 0; j < locationBlocks.size(); ++j) {
            LocationConfig loc = saveLocationConfig(locationBlocks[j]);
            config.addLocation(loc);
        }

        this->servers.push_back(config);
    }
}

ServerConfig ConfigParser::saveServerConfig(const std::vector<Token>& tokens) {
    ServerConfig config;

    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i].type != KEY) continue;

        std::string key = tokens[i].value;

        if(key=="server"){
            continue;
        }
        if (key == "listen") {
            if (i + 2 >= tokens.size() || tokens[i + 1].type != VALUE || tokens[i + 2].type != SEMICOLON)
                throw std::runtime_error("Invalid 'listen' directive syntax");
            config.setPort(std::atoi(tokens[i + 1].value.c_str()));
            i += 2;
        }
        else if (key == "server_name") {
            if (i + 2 >= tokens.size() || tokens[i + 1].type != VALUE || tokens[i + 2].type != SEMICOLON)
                throw std::runtime_error("Invalid 'server_name' directive syntax");
            config.setServerName(tokens[i + 1].value);
            i += 2;
        }
        else if (key == "root") {
            if (i + 2 >= tokens.size() || tokens[i + 1].type != VALUE || tokens[i + 2].type != SEMICOLON)
                throw std::runtime_error("Invalid 'root' directive syntax");
            config.setRoot(tokens[i + 1].value);
            i += 2;
        }
        else if (key == "error_page") {
            if (i + 3 >= tokens.size() || tokens[i + 1].type != VALUE || tokens[i + 2].type != VALUE || tokens[i + 3].type != SEMICOLON)
                throw std::runtime_error("Invalid 'error_page' directive syntax");
            int code = std::atoi(tokens[i + 1].value.c_str());
            std::string path = tokens[i + 2].value;
            config.addErrorPage(code, path);
            i += 3;
        }
        else {
            while (i < tokens.size() && tokens[i].type != SEMICOLON) ++i;
        }
    }
    return config;
}

LocationConfig ConfigParser::saveLocationConfig(const LocationBlock& lb) {
    
    if (lb.path.empty() || lb.path[0] != '/')
        throw std::runtime_error("Location path is invalid: must start with '/' and not be empty");
    LocationConfig config;
    config.setPath(lb.path);

    const std::vector<Token>& tokens = lb.tokens;

    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i].type != KEY)
            continue;

        std::string key = tokens[i].value;

        if (key == "root") {
            if (i + 2 >= tokens.size() || tokens[i + 1].type != VALUE || tokens[i + 2].type != SEMICOLON)
                throw std::runtime_error("Invalid 'root' directive in location block");
            config.setRoot(tokens[i + 1].value);
            i += 2;
        }
        else if (key == "index") {
            if (i + 2 >= tokens.size() || tokens[i + 1].type != VALUE || tokens[i + 2].type != SEMICOLON)
                throw std::runtime_error("Invalid 'index' directive in location block");
            config.setIndex(tokens[i + 1].value);
            i += 2;
        }
        else if (key == "autoindex") {
            if (i + 2 >= tokens.size() || tokens[i + 1].type != VALUE || tokens[i + 2].type != SEMICOLON)
                throw std::runtime_error("Invalid 'autoindex' directive in location block");
            std::string val = tokens[i + 1].value;
            if (val != "on" && val != "off")
                throw std::runtime_error("autoindex must be 'on' or 'off'");
            config.setAutoindex(val == "on");
            i += 2;
        }
        else if (key == "cgi_path") {
            if (i + 2 >= tokens.size() || tokens[i + 1].type != VALUE || tokens[i + 2].type != SEMICOLON)
                throw std::runtime_error("Invalid 'cgi_path' directive in location block");
            config.setCgiPath(tokens[i + 1].value);
            i += 2;
        }
        else if (key == "upload_path") {
            if (i + 2 >= tokens.size() || tokens[i + 1].type != VALUE || tokens[i + 2].type != SEMICOLON)
                throw std::runtime_error("Invalid 'upload_path' directive in location block");
            config.setUploadPath(tokens[i + 1].value);
            i += 2;
        }
        else if (key == "max_body_size") {
            if (i + 2 >= tokens.size() || tokens[i + 1].type != VALUE || tokens[i + 2].type != SEMICOLON)
                throw std::runtime_error("Invalid 'max_body_size' directive");
            int size = std::atoi(tokens[i + 1].value.c_str());
            if (size <= 0)
                throw std::runtime_error("max_body_size must be positive");
            config.setMaxBodySize(size);
            i += 2;
        }
        else if (key == "return") {
            if (i + 3 >= tokens.size() || tokens[i + 1].type != VALUE || tokens[i + 2].type != VALUE || tokens[i + 3].type != SEMICOLON)
                throw std::runtime_error("Invalid 'return' directive in location block");
            int code = std::atoi(tokens[i + 1].value.c_str());
            if (code != 301 && code != 302 && code != 307)
                throw std::runtime_error("Invalid return code: " + tokens[i + 1].value);
            config.setReturnCode(code);
            config.setRedirectPath(tokens[i + 2].value);
            i += 3;
        }        
        else if (key == "method") {
            std::vector<std::string> methods;
            ++i;
            while (i < tokens.size() && tokens[i].type == VALUE) {
                const std::string& m = tokens[i].value;
                if (m != "GET" && m != "POST" && m != "DELETE")
                    throw std::runtime_error("Invalid HTTP method in 'method' directive: " + m);
                methods.push_back(m);
                ++i;
            }
            if (i >= tokens.size() || tokens[i].type != SEMICOLON)
                throw std::runtime_error("Missing semicolon after 'method' directive");
            config.setMethods(methods);
        }
        else {
            while (i < tokens.size() && tokens[i].type != SEMICOLON)
                ++i;
        }
    }
    return config;
}

const std::vector<ServerConfig>& ConfigParser::getServers() const {
    return this->servers;
}
