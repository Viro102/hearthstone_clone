#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <algorithm>
#include <cstring>
struct Client {
    int socket;
    bool isReady = false;
};

class Server {
public:
    Server(int port) : _port(port) {
        startServer();
    }

    ~Server() {
        close(_server_fd);
    }

private:
    int _port;
    int _server_fd;
    std::vector<Client> _clients;
    std::mutex _clients_mutex;

    void startServer() {
        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);

        _server_fd = socket(AF_INET, SOCK_STREAM, 0);
        setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(_port);

        bind(_server_fd, (struct sockaddr *)&address, sizeof(address));
        listen(_server_fd, 10);

        std::thread(&Server::acceptClients, this).detach();
    }

    void acceptClients() {
        int clientSocket;
        struct sockaddr_in address;
        int addrlen = sizeof(address);

        while (true) {
            clientSocket = accept(_server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
            std::cout << "New client connected: Socket " << clientSocket << std::endl; // Log new connection
            std::thread(&Server::handleClient, this, clientSocket).detach();
        }
    }

    void handleClient(int clientSocket) {
        Client newClient;
        newClient.socket = clientSocket;

        _clients_mutex.lock();
        _clients.push_back(newClient);
        _clients_mutex.unlock();

        char buffer[1024] = {0};
        while (true) {
            int valread = read(clientSocket, buffer, 1024);
            if (valread <= 0) break; // Client disconnected

            std::cout << "Received message from client " << clientSocket << ": " << buffer << std::endl; // Log received message

            std::string message(buffer, valread); // Create a string with the received data
            processClientMessage(clientSocket, message);

            memset(buffer, 0, 1024); // Reset buffer
        }

        removeClient(clientSocket);
    }

    void processClientMessage(int clientSocket, const std::string& message) {
        if (message.find("ready") != std::string::npos) { // Check if message contains "ready"
            _clients_mutex.lock();
            for (auto& client : _clients) {
                if (client.socket == clientSocket) {
                    client.isReady = true;
                    std::cout << "Client " << clientSocket << " marked as ready." << std::endl; // Log ready status
                    break;
                }
            }
            _clients_mutex.unlock();
        }

        checkForGamePairing();
    }

    void removeClient(int clientSocket) {
        _clients_mutex.lock();
        _clients.erase(std::remove_if(_clients.begin(), _clients.end(),
                                      [clientSocket](const Client& client) { return client.socket == clientSocket; }),
                       _clients.end());
        _clients_mutex.unlock();
        close(clientSocket);
    }

    void checkForGamePairing() {
        _clients_mutex.lock();
        std::vector<Client*> readyClients;
        for (auto& client : _clients) {
            if (client.isReady) {
                readyClients.push_back(&client);
                if (readyClients.size() == 2) {
                    startGame(*readyClients[0], *readyClients[1]);
                    break;
                }
            }
        }
        _clients_mutex.unlock();
    }

    void startGame(Client& player1, Client& player2) {
        std::cout << "Starting game between clients: " << player1.socket << " and " << player2.socket << std::endl;

    }
};
