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

std::vector<std::vector<Token> > ConfigParser::extractServerBlock(const std::vector<Token>& tokens){
    std::vector<std::vector<Token> > block;
    std::vector<Token> currentBlock;
    int braceSize = 0;
    int insideServer = false;

    for(size_t i = 0;i < tokens.size(); ++i){
        const Token& tok = tokens[i];
        if(!insideServer && tok.type == KEY && tok.value == "server"){
            insideServer = true;
        }
        if(insideServer){
            currentBlock.push_back(tok);
            if(tok.type == BRACE_OPEN) braceSize++;
            if(tok.type == BRACE_CLOSE) braceSize--;

            if(braceSize == 0&&tok.type == BRACE_CLOSE){
                block.push_back(currentBlock);
                currentBlock.clear();
                insideServer = false;
            }
        }
    }
    return block;
}

std::vector<LocationBlock> ConfigParser::extractLocationBlock(const std::vector<Token>& serverBlock){
    std::vector<LocationBlock> locationBlocks;
    int braceSize = 0;
    bool insideLocation = false;
    std::string path;
    std::vector<Token> currentBlock;

    for(size_t i = 0; i< serverBlock.size(); ++i){
        const Token& tok = serverBlock[i];
        if (!insideLocation && tok.type == KEY && tok.value == "location") {
            if (i + 2 < serverBlock.size() &&
                serverBlock[i + 1].type == VALUE &&
                serverBlock[i + 2].type == BRACE_OPEN) {
                path = serverBlock[i + 1].value;
                i += 2;
                braceSize = 1;
                insideLocation = true;
                currentBlock.clear();
                continue;
            } else {
                throw std::runtime_error("Missing Location block");
            }
        }        
        if (insideLocation) {
            currentBlock.push_back(tok);
            if (tok.type == BRACE_OPEN) braceSize++;
            if (tok.type == BRACE_CLOSE) braceSize--;
    
            if (braceSize == 0) {
                LocationBlock lb;
                lb.path = path;
                lb.tokens = currentBlock;
                locationBlocks.push_back(lb);
                insideLocation = false;
            }
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
            insideLocation = true;

            if (i + 2 < serverBlock.size()
                && serverBlock[i + 1].type == VALUE
                && serverBlock[i + 2].type == BRACE_OPEN) {
                i += 2;
                braceDepth = 1;
                continue;
            } else {
                throw std::runtime_error("Invalid location block syntax");
            }
        }

        if (insideLocation) {
            if (tok.type == BRACE_OPEN) braceDepth++;
            if (tok.type == BRACE_CLOSE) braceDepth--;
            if (braceDepth == 0) {
                insideLocation = false;
            }
            continue;
        }
        result.push_back(tok);
    }
    return result;
}

void ConfigParser::Parse(const std::string& filepath){
    std::string raw = readfile(filepath);
    std::string noComment = removeComment(raw);
    std::string cleaned = trimWhitespace(noComment);
    std::vector<Token> tokens = tokenize(cleaned);
    std::vector<std::vector<Token> > serverBlocks = extractServerBlock(tokens);
    std::vector<ServerBlock> parsedServers;

    for (size_t i = 0; i < serverBlocks.size(); i++) {
        std::vector<Token> serverBlock = serverBlocks[i];
        std::vector<Token> serverConfigTokens = filterNonLocationToken(serverBlock);
        std::vector<LocationBlock> locationBlocks = extractLocationBlock(serverBlock);
    
        ServerBlock sb;
        sb.configTokens = serverConfigTokens;
        sb.locations = locationBlocks;
    
        parsedServers.push_back(sb);
    }
    this->servers = parsedServers;
}

const std::vector<ServerBlock>& ConfigParser::getParsedServers() const {
    return servers;
}