#pragma once
#include "Exceptions.hpp"
#include "Storage.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace astrodb {

/* ---------- simple in-memory table ---------- */
struct MemTable {
    std::vector<std::string>              columns;
    std::vector<std::vector<std::string>> rows;
};

extern std::unordered_map<std::string, MemTable> gMemDB;
extern storage::FileManager                      gFileMgr;

/* ---------- Command hierarchy ---------- */
class SQLCommand {
public:
    virtual ~SQLCommand() = default;
    virtual void execute() = 0;
};

class CreateTableCmd : public SQLCommand {
    std::string               name_;
    std::vector<std::string>  cols_;
public:
    CreateTableCmd(std::string n, std::vector<std::string> c);
    void execute() override;
};

class InsertCmd : public SQLCommand {
    std::string               name_;
    std::vector<std::string>  values_;
public:
    InsertCmd(std::string n, std::vector<std::string> v);
    void execute() override;
};

class SelectCmd : public SQLCommand {
    std::string name_;
public:
    explicit SelectCmd(std::string n);
    void execute() override;
};

} // namespace astrodb
