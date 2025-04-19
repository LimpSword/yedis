#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <ostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

#include "include/network.h"

static int32_t query(const int fd, const char *text) {
    if (const int err = write_line(fd, text)) {
        return err;
    }

    auto rbuf = new char[k_max_msg + 1];
    if (const int err = read_line(fd, rbuf)) {
        return err;
    }
    printf("server says: %s\n", rbuf);
    delete[] rbuf;
    return 0;
}

int main() {
    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        std::cout << "socket error" << std::endl;
        return 1;
    }

    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(29863);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);
    if (int rv = connect(fd, reinterpret_cast<const struct sockaddr *>(&addr), sizeof(addr))) {
        std::cout << "connect error" << std::endl;
        return 1;
    }

    while (true) {
        char buf[k_max_msg + 1];
        std::cout << "yedis> ";
        std::cin.getline(buf, k_max_msg + 1);
        if (const int err = query(fd, buf)) {
            break;
        }
    }

    close(fd);
    return 0;
}
