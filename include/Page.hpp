#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <functional>

namespace elvoiddb::storage {

inline constexpr size_t PAGE_SIZE = 4096;

struct PageHeader {
    uint16_t slotCount;   // number of records in page
    uint16_t freeOffset;  // start of free space (grows upward)
};

class Page {
    char data[PAGE_SIZE]{};
public:
    Page();

    // insert raw record bytes; returns slot index or -1 if not enough space
    int insertRecord(const std::string &bytes);

    // iterate over every record in insertion order
    void forEachRecord(const std::function<void(const char *, uint16_t)> &cb) const;

    // expose raw buffer (needed by BlockFile I/O)
    const char *raw() const { return data; }
    char *raw() { return data; }
};

} // namespace elvoiddb::storage
