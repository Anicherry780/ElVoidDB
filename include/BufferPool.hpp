#pragma once
#include "Page.hpp"
#include <unordered_map>
#include <list>
#include <filesystem>
#include <mutex>

namespace astrodb::storage {

namespace fs = std::filesystem;

struct PageId {
    fs::path path;   // table file path
    size_t    no;    // page number

    bool operator==(const PageId &o) const { return no==o.no && path==o.path; }
};

struct PageIdHash {
    size_t operator()(const PageId &p) const noexcept {
        return std::hash<std::string>{}(p.path.string()) ^ std::hash<size_t>{}(p.no << 1);
    }
};

/*  Simple LRU buffer pool (not thread-safe yet)  */
class BufferPool {
    struct Frame {
        Page     page;
        PageId   id;
        bool     dirty{false};
        uint32_t pin{0};
    };

    size_t max_;                                   // max frames
    std::list<Frame> lru_;                         // front = most recent
    std::unordered_map<PageId, std::list<Frame>::iterator, PageIdHash> map_;
    std::mutex mtx_;

    // helper: write page back to disk
    void flushFrame(const Frame &f) const;

public:
    explicit BufferPool(size_t m = 64) : max_(m) {}

    // fetch page: loads from disk if absent; pins it and returns reference
    Page &get(const fs::path &file, size_t pageNo);

    // mark page as dirty (caller changed it)
    void markDirty(const fs::path &file, size_t pageNo);

    // unpin when caller done
    void unpin(const fs::path &file, size_t pageNo);

    // flush & drop all frames (called at shutdown)
    void flushAll();
};

extern BufferPool gBufPool;   // global instance

} // namespace astrodb::storage
