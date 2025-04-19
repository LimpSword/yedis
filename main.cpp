#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "include/network.h"


static int32_t one_request(int connfd) {
    auto rbuf = new char[k_max_msg];
    if (const int err = read_line(connfd, rbuf)) {
        return err;
    }
    std::cout << "client says: " << rbuf << std::endl;
    delete[] rbuf;

    constexpr char reply[] = "world";
    if (const int err = write_line(connfd, reply)) {
        return err;
    }
    return 0;
}


[[noreturn]] int main() {
    std::cout << "Hello, World!" << std::endl;

    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    constexpr int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(29863);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (const int rv = bind(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)); rv < 0) {
        std::cout << "bind error" << std::endl;
        return 1;
    }

    if (const int rv = listen(fd, SOMAXCONN); rv < 0) {
        std::cout << "listen error" << std::endl;
        return 1;
    }

    while (true) {
        sockaddr_in client_addr{};
        socklen_t addrlen = sizeof(client_addr);
        int connfd = accept(fd, reinterpret_cast<struct sockaddr *>(&client_addr), &addrlen);
        if (connfd < 0) {
            std::cout << "accept error " << errno << std::endl;
            continue;
        }

        while (true) {
            int32_t err = one_request(connfd);
            if (err) {
                break;
            }
        }
        close(connfd);
    }
}
