#ifndef NETWORK_H
#define NETWORK_H
#include <cassert>
#include <unistd.h>
#include <sys/_types/_ssize_t.h>

constexpr size_t k_max_msg = 4096;

inline int32_t read_full(const int fd, char *buf, size_t n) {
    while (n > 0) {
        const ssize_t rv = read(fd, buf, n);
        if (rv <= 0) {
            return -1; // error, or unexpected EOF
        }
        assert(static_cast<size_t>(rv) <= n);
        n -= static_cast<size_t>(rv);
        buf += rv;
    }
    return 0;
}

inline int32_t write_all(const int fd, const char *buf, size_t n) {
    while (n > 0) {
        const ssize_t rv = write(fd, buf, n);
        if (rv <= 0) {
            return -1; // error
        }
        assert(static_cast<size_t>(rv) <= n);
        n -= static_cast<size_t>(rv);
        buf += rv;
    }
    return 0;
}


inline int read_line(const int connfd, char *buf) {
    // 4 bytes header
    char rbuf[4 + k_max_msg];
    int32_t err = read_full(connfd, rbuf, 4);
    if (err) {
        std::cout << "read error " << errno << std::endl;
        return err;
    }
    uint32_t len = 0;
    memcpy(&len, rbuf, 4);
    if (len > k_max_msg) {
        std::cout << "message too long" << std::endl;
        return -1;
    }
    // request body
    err = read_full(connfd, &rbuf[4], len);
    if (err) {
        std::cout << "read error" << std::endl;
        return err;
    }
    memcpy(buf, &rbuf[4], len);
    return 0;
}

inline int write_line(const int connfd, const char *buf) {
    if (strlen(buf) > k_max_msg) {
        return -1;
    }

    // 4 bytes header
    char wbuf[4 + k_max_msg];
    const auto len = static_cast<uint32_t>(strlen(buf));
    memcpy(wbuf, &len, 4);
    memcpy(&wbuf[4], buf, len);
    return write_all(connfd, wbuf, 4 + len);
}

#endif //NETWORK_H
