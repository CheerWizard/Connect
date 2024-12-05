//
// Created by cheerwizard on 03.12.24.
//

#include "Networking.hpp"

NetworkClient::NetworkClient(const string& host, int port)
:
thread("NetworkClient", ThreadPriority_HIGHEST),
host(host),
port(port)
{
    connect();
}

NetworkClient::~NetworkClient()
{
    disconnect();
}

void NetworkClient::connect() {
    connectionSocket.connect(host, port);
}

void NetworkClient::disconnect() {
    connectionSocket.disconnect();
}

void NetworkClient::send(const string& data) {
    connectionSocket.send(data);
}

string NetworkClient::receive() {
    return connectionSocket.receive();
}