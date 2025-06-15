#include "Parser.hpp"
#include "BufferPool.hpp"
#include <iostream>

using elvoiddb::Parser;
using elvoiddb::AstroDBException;

int main() {
    std::cout << "ElVoidDB> ";
    std::string line;

    while (std::getline(std::cin, line)) {
        if (line.empty()) { std::cout << "ElVoidDB> "; continue; }

        try {
            auto cmd = Parser::parse(line);
            if (!cmd) break;                     // EXIT / QUIT
            cmd->execute();
        } catch (const AstroDBException& e) {
            std::cout << "Error: " << e.what() << '\n';
        }

        std::cout << "ElVoidDB> ";
    }
    elvoiddb::storage::gBufPool.flushAll();
    std::cout << "Bye from ElVoidDB!\n";
    return 0;
}
