#pragma once
#include "Exceptions.hpp"
#include "Page.hpp"
#include <filesystem>
#include <fstream>
#include <memory>
#include <unordered_map>
#include <vector>

namespace astrodb::storage {

namespace fs  = std::filesystem;

/* ─── BlockFile: raw 4 KB pages on disk ────────────────────── */
class BlockFile {
    fs::path    path_;
    mutable std::fstream file_;
public:
    BlockFile(const fs::path& p, bool create);
    void   writePage(size_t pageNo, const Page& pg);
    void   readPage (size_t pageNo, Page& pg) const;
    size_t pageCount();
};

/* ─── TableFile: metadata + data pages ─────────────────────── */
class TableFile {
    BlockFile bf_;
public:
    TableFile(const std::string& table, bool create,
              const std::vector<std::string>& cols = {});

    void appendRow  (const std::vector<std::string>& row);          // INSERT
    void loadAllRows(std::vector<std::vector<std::string>>& dest);  // full table scan

    BlockFile& bf() { return bf_; }
    std::vector<std::string> columnList() const;      // parse page-0 header

private:
    static std::string              serializeRow  (const std::vector<std::string>& row);
    static std::vector<std::string> deserializeRow(const char* data, uint16_t len);
};

/* ─── FileManager: keeps TableFile objects open ────────────── */
class FileManager {
    std::unordered_map<std::string,std::unique_ptr<TableFile>> open_;
public:
    void        createTable(const std::string& name,
                            const std::vector<std::string>& cols);
    TableFile*  openTable  (const std::string& name);
};

} // namespace astrodb::storage
