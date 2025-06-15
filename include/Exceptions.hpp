#pragma once
#include <stdexcept>
#include <string>

namespace elvoiddb {

// Base for all engine-level errors
class AstroDBException : public std::runtime_error {
public:
    explicit AstroDBException(const std::string& msg)
        : std::runtime_error(msg) {}
};

class StorageError   : public AstroDBException { using AstroDBException::AstroDBException; };
class ParseError     : public AstroDBException { using AstroDBException::AstroDBException; };
class ExecutionError : public AstroDBException { using AstroDBException::AstroDBException; };

} // namespace elvoiddb
