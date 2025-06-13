#include "Parser.hpp"
#include "BufferPool.hpp"
#include <iostream>

using astrodb::Parser;
using astrodb::AstroDBException;

int main() {
    std::cout << "AstroDB> ";
    std::string line;

    while (std::getline(std::cin, line)) {
        if (line.empty()) { std::cout << "AstroDB> "; continue; }

        try {
            auto cmd = Parser::parse(line);
            if (!cmd) break;                     // EXIT / QUIT
            cmd->execute();
        } catch (const AstroDBException& e) {
            std::cout << "Error: " << e.what() << '\n';
        }

        std::cout << "AstroDB> ";
    }
    astrodb::storage::gBufPool.flushAll();
    std::cout << "Bye from AstroDB!\n";
    return 0;
}
