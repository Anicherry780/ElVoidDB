#include "Commands.hpp"
#include <iostream>
#include <algorithm>

namespace elvoiddb {

std::unordered_map<std::string, MemTable> gMemDB;
storage::FileManager                       gFileMgr;

/* helpers */
static void printRow(const std::vector<std::string>& row)
{
    for (size_t i = 0; i < row.size(); ++i)
        std::cout << row[i] << (i + 1 == row.size() ? '\n' : '\t');
}

/* CREATE TABLE */
CreateTableCmd::CreateTableCmd(std::string n, std::vector<std::string> c)
    : name_(std::move(n)), cols_(std::move(c)) {}

void CreateTableCmd::execute()
{
    if (gMemDB.count(name_)) throw ExecutionError("table exists in mem");
    gFileMgr.createTable(name_, cols_);
    gMemDB[name_] = MemTable{cols_, {}};
    std::cout << "Table '" << name_ << "' created.\n";
}

static MemTable& ensureLoaded(const std::string& name)
{
    auto it = gMemDB.find(name);
    if (it == gMemDB.end()) {
        auto* tf = gFileMgr.openTable(name);
        if (!tf) throw ExecutionError("no such table");

        auto cols = tf->columnList();
        if (cols.empty()) throw ExecutionError("corrupt table header");

        it = gMemDB.emplace(name, MemTable{cols, {}}).first;
        tf->loadAllRows(it->second.rows);              // first (and only) load
    }
    return it->second;
}


/* INSERT INTO */
InsertCmd::InsertCmd(std::string n, std::vector<std::string> v)
    : name_(std::move(n)), values_(std::move(v)) {}

void InsertCmd::execute()
{
    auto& tbl = ensureLoaded(name_);

    if (values_.size() != tbl.columns.size())
        throw ExecutionError("column count mismatch");

    tbl.rows.push_back(values_);                       // RAM
    if (auto* tf = gFileMgr.openTable(name_))
        tf->appendRow(values_);                        // disk

    std::cout << "1 row inserted.\n";
}

/* SELECT * FROM */
SelectCmd::SelectCmd(std::string n) : name_(std::move(n)) {}

void SelectCmd::execute()
{
    auto& tbl = ensureLoaded(name_);

    printRow(tbl.columns);
    for (const auto& r : tbl.rows) printRow(r);
}

} // namespace elvoiddb
