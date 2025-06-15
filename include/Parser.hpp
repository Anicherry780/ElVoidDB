#pragma once
#include "Commands.hpp"
#include <memory>

namespace elvoiddb {

class Parser {
    static void upper(std::string& s);
public:
    static std::unique_ptr<SQLCommand> parse(const std::string& line);
};

} // namespace elvoiddb
