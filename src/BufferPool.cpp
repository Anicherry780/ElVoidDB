#include "BufferPool.hpp"
#include "Storage.hpp"   // for BlockFile
#include <fstream>
#include <cstring>
#include "ThreadPool.hpp"

namespace elvoiddb::storage {

BufferPool gBufPool; // default 64 frames

static void rawRead(const fs::path &p, size_t n, Page &pg) {
    std::ifstream f(p, std::ios::binary);
    if (!f) {                               // file not yet created
        std::memset(pg.raw(), 0, PAGE_SIZE);
        return;
    }
    f.seekg(n * PAGE_SIZE);
    f.read(pg.raw(), PAGE_SIZE);
    std::streamsize got = f.gcount();
    if (got < PAGE_SIZE)                    // short read → zero‐fill remainder
        std::memset(pg.raw() + got, 0, PAGE_SIZE - got);
}

static void rawWrite(const fs::path &p, size_t n, const Page &pg) {
    std::fstream f(p, std::ios::binary | std::ios::in | std::ios::out);
    if (!f) throw StorageError("rawWrite open fail");
    f.seekp(n * PAGE_SIZE);
    f.write(pg.raw(), PAGE_SIZE);
    if (!f) throw StorageError("rawWrite fail");
    f.flush();
}

void BufferPool::flushFrame(const Frame &f) const {
    rawWrite(f.id.path, f.id.no, f.page);
}

Page &BufferPool::get(const fs::path &file, size_t n) {
    std::scoped_lock lock(mtx_);
    PageId id{file, n};
    if (auto it = map_.find(id); it != map_.end()) {
        lru_.splice(lru_.begin(), lru_, it->second); // MRU
        it->second->pin++;
        return it->second->page;
    }

    if (lru_.size() >= max_) {
        auto rit = lru_.rbegin();
        while (rit != lru_.rend() && rit->pin != 0) ++rit;
        if (rit == lru_.rend()) throw StorageError("all pages pinned");
        if (rit->dirty) flushFrame(*rit);
        map_.erase(rit->id);
        lru_.erase(std::next(rit).base());
    }

    lru_.push_front(Frame{});
    Frame &f = lru_.front();
    f.id = id; f.pin = 1; f.dirty = false;
    rawRead(file, n, f.page);
    map_[id] = lru_.begin();
    return f.page;
}

void BufferPool::markDirty(const fs::path &file, size_t n) {
    std::scoped_lock lock(mtx_);
    if (auto it = map_.find(PageId{file, n}); it != map_.end()) it->second->dirty = true;
}

void BufferPool::unpin(const fs::path &file, size_t n) {
    std::scoped_lock lock(mtx_);
    auto it = map_.find(PageId{file, n});
    if (it != map_.end() && it->second->pin) {
        it->second->pin--;
        /*  ── NEW: if page is dirty and no pins left, flush async ── */
        if (it->second->pin == 0 && it->second->dirty) {
            Frame *fptr = &(*it->second);            // capture raw ptr
            util::gThreadPool.submit([this, fptr] {
                std::scoped_lock lk(mtx_);
                flushFrame(*fptr);
                fptr->dirty = false;
            });
        }
    }
}

void BufferPool::flushAll() {
    std::scoped_lock lock(mtx_);
    for (auto &f : lru_) if (f.dirty) flushFrame(f);
}

} // namespace elvoiddb::storage
