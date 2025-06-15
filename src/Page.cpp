#include "Page.hpp"
#include <cstring>

namespace elvoiddb::storage {

Page::Page()
{
    auto* h = reinterpret_cast<PageHeader*>(data);
    h->slotCount  = 0;
    h->freeOffset = sizeof(PageHeader);          // free space begins right after header
}

int Page::insertRecord(const std::string& bytes)
{
    auto* h = reinterpret_cast<PageHeader*>(data);

    uint16_t slotArrayEnd   = sizeof(PageHeader) + h->slotCount * sizeof(uint16_t);
    uint16_t freeStart      = h->freeOffset;
    uint16_t freeEnd        = PAGE_SIZE - slotArrayEnd;      // record area grows downward
    uint16_t need           = bytes.size() + sizeof(uint16_t);

    if (freeStart + need > freeEnd) return -1;               // not enough space

    // write len + payload
    auto* lenPtr = reinterpret_cast<uint16_t*>(data + freeStart);
    *lenPtr = static_cast<uint16_t>(bytes.size());
    std::memcpy(data + freeStart + sizeof(uint16_t), bytes.data(), bytes.size());

    // slot entry
    auto* slotPtr =
        reinterpret_cast<uint16_t*>(data + sizeof(PageHeader) + h->slotCount * sizeof(uint16_t));
    *slotPtr = freeStart;

    h->slotCount += 1;
    h->freeOffset += need;
    return h->slotCount - 1;
}

void Page::forEachRecord(const std::function<void(const char*,uint16_t)>& cb) const
{
    const auto* h = reinterpret_cast<const PageHeader*>(data);
    for (uint16_t i = 0; i < h->slotCount; ++i) {
        auto* slotPtr = reinterpret_cast<const uint16_t*>(
            data + sizeof(PageHeader) + i * sizeof(uint16_t));
        uint16_t offset = *slotPtr;
        auto* lenPtr = reinterpret_cast<const uint16_t*>(data + offset);
        uint16_t len = *lenPtr;
        const char* payload = data + offset + sizeof(uint16_t);
        cb(payload, len);
    }
}

} // namespace elvoiddb::storage
