#ifndef BUFFER_H
#define BUFFER_H

#include <vector>
#include <span>

struct Buffer {
    std::vector<uint8_t> storage;
    size_t start = 0; // where valid data begins
    size_t end = 0; // where valid data ends

    void append(const uint8_t *data, size_t len) {
        storage.insert(storage.end(), data, data + len);
        end += len;
    }

    [[nodiscard]] std::span<const uint8_t> data() const {
        return {storage.data() + start, size()};
    }

    [[nodiscard]] size_t size() const {
        return end - start;
    }

    [[nodiscard]] bool empty() const {
        return size() == 0;
    }

    void consume(size_t n) {
        if (n > size()) {
            n = size();
        }

        start += n;
        if (start == end) {
            // All data consumed
            start = 0;
            end = 0;
            storage.clear();
        } else if (start > 4096) {
            // Compact the buffer when start offset gets too large
            std::vector tmp(storage.begin() + start, storage.begin() + end);
            storage = std::move(tmp);
            end -= start;
            start = 0;
        }
    }
};

#endif // BUFFER_H
