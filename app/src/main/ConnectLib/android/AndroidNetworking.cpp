//
// Created by cheerwizard on 05.12.24.
//

#include "Networking.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>

const int SOCKET_NULL = -1;

TcpSocket::TcpSocket() {
    if ((handle = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        handle = SOCKET_NULL;
    }
}

TcpSocket::~TcpSocket() {
    disconnect();
}

bool TcpSocket::connect(const string &host, int port) {
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0) {
        LOG_ERR("");
        return false;
    }

    if (::connect(handle, (sockaddr*) &addr, sizeof(addr)) < 0) {
        LOG_ERR("Failed to connect to %s:%i", host, port);
        return false;
    }

    return true;
}

void TcpSocket::disconnect() {
    if (handle != SOCKET_NULL) {
        ::close(handle);
    }
}

void TcpSocket::send(const string &data) {
    ::send(handle, data.c_str(), data.size(), 0);
}

string TcpSocket::receive() {
    // TODO: temp code, will be removed. need to check if we all data we need.
    char buffer[1024] = {0};
    int result = ::read(handle, buffer, 1024);
    if (result != -1) {
        return buffer;
    } else {
        return "Error";
    }
}