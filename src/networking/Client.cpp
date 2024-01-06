#include <Client.h>

Client::Client(int socketFD) : m_socket(socketFD) {}

Client::~Client() {
    shutdownClient();
}

int Client::startClient(short port) {
    struct sockaddr_in serverAddress{};

    if ((m_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "\n Socket creation error \n";
        return -1;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0) {
        cout << "\nInvalid address/Address not supported\n";
        return -1;
    }

    if (connect(m_socket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        cout << "\nConnection Failed\n";
        return -1;
    }

    std::jthread(&Client::listenToServer, this).detach();
    return 0;
}

void Client::listenToServer() {
    while (true) {
        char buffer[1024] = {0};
        long valread = recv(m_socket, buffer, 1024, 0);
        if (valread > 0) {
            string message(buffer, valread);
            updateLocalLobbyState(message);
        }
    }
}

void Client::updateLocalLobbyState(const string &message) {
    nlohmann::json json = nlohmann::json::parse(message);

    m_lobbyState.players.clear();
    for (const auto &playerJson: json["players"]) {
        LobbyState::PlayerInfo player;
        player.name = playerJson["name"];
        player.isReady = playerJson["isReady"];
        m_lobbyState.players.push_back(player);
    }
}


void Client::shutdownClient() const {
    if (m_socket >= 0) {
        close(m_socket);
    }
}

int Client::getSocket() const {
    return m_socket;
}

bool Client::isReady() const {
    return m_isReady;
}

void Client::setReady(bool ready) {
    m_isReady = ready;
}

LobbyState Client::getLobbyState() {
    return m_lobbyState;
}
