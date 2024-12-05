//
// Created by cheerwizard on 03.12.24.
//

#ifndef CONNECT_NETWORKING_HPP
#define CONNECT_NETWORKING_HPP

extern const int SOCKET_NULL;

class TcpSocket {

public:
    TcpSocket();
    ~TcpSocket();

    bool connect(const string& host, int port);
    void disconnect();

    void send(const string& data);
    string receive();

private:
    int handle = SOCKET_NULL;

};

class NetworkClient {

public:
    NetworkClient(const string& host, int port);
    ~NetworkClient();

    void send(const string& data);
    string receive();

private:
    void connect();
    void disconnect();

    TcpSocket connectionSocket;
    Thread thread;
    string host;
    int port;
};

#endif //CONNECT_NETWORKING_HPP