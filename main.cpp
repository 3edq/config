#include "ConfigParser.hpp"

int main() {
    ConfigParser parser;
    try {
        parser.Parse("example.conf");
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}