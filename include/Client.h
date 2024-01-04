#pragma once

#include "Common.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

class Client {
public:
    static int startClient() {
        int sock;
        struct sockaddr_in serv_addr{};

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            cout << "\n Socket creation error \n";
            return -1;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(8080);

        // Convert IPv4 and IPv6 addresses from text to binary form
        if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
            cout << "\nInvalid address/Address not supported\n";
            return -1;
        }

        if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            cout << "\nConnection Failed\n";
            return -1;
        }

        // Example of sending a message to the server
        std::string message = "Hello from Client!";
        send(sock, message.c_str(), message.size(), 0);

        return sock;
    }
};
