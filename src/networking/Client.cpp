#include <Client.h>

Client::Client(int socketFD) : m_socket(socketFD) {}

Client::~Client() {
    shutdown();
}

int Client::start(short port, const string &ipAddr) {
    struct sockaddr_in serverAddress{};

    if ((m_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "\n Socket creation error \n";
        return -1;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, ipAddr.c_str(), &serverAddress.sin_addr) <= 0) {
        cout << "\nInvalid address/Address not supported\n";
        return -1;
    }

    if (connect(m_socket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        cout << "\nConnection Failed\n";
        return -1;
    }

    m_serverListener = std::jthread(&Client::listenToServer, this);
    return 0;
}

bool Client::recvAll(int socket, char *buffer, size_t length) {
    size_t totalReceived = 0;
    ssize_t lastReceived;
    while (totalReceived < length) {
        lastReceived = recv(socket, buffer + totalReceived, length - totalReceived, 0);
        if (lastReceived == -1) {
            std::cerr << "Failed to receive data." << std::endl;
            return false;
        }
        if (lastReceived == 0) {
            std::cerr << "The peer has performed an orderly shutdown." << std::endl;
            return false;
        }
        totalReceived += lastReceived;
    }
    return true;
}

void Client::listenToServer() {
    while (true) {
        // First, read the length of the message
        uint32_t messageLength;
        if (!recvAll(m_socket, reinterpret_cast<char *>(&messageLength), sizeof(messageLength))) {
            std::cerr << "Failed to receive message length." << std::endl;
            break;
        }
        messageLength = ntohl(messageLength);  // Convert length from network byte order to host byte order

        // Now read the message of that length
        std::string message;
        message.resize(messageLength);
        if (!recvAll(m_socket, &message[0], messageLength)) {
            std::cerr << "Failed to receive message." << std::endl;
            break;
        }

        processMessage(message);
    }
}

void Client::sendMessage(const string &message, const json &data) const {
    json j;
    j["type"] = message;
    j["data"] = data;
    string serializedMsg = j.dump();

    cout << "Client sending message: " << j.dump(4) << endl;

    send(m_socket, serializedMsg.c_str(), serializedMsg.size(), 0);
}

void Client::setStateChangeCallback(const StateChangeCallback &callback) {
    stateChangeCallback = callback;
}

void Client::processMessage(const string &message) {
    try {
        json j = json::parse(message);

        string type = j["type"];
        string data = j["data"].dump();

        cout << "Client received message = " << j.dump(4) << endl;

        if (type == "updateLobbyState") {
            updateLocalLobbyState(data);
        }

        if (type == "yourID") {
            m_ID = std::stoi(data);
        }

        if (type == "startGame" && stateChangeCallback) {
            updateLocalGameplayState(data);
            stateChangeCallback(GameState::GAMEPLAY);
        }

        if (type == "updateGameState") {
            updateLocalGameplayState(data);
        }

        if (type == "opponentDisconnected" && stateChangeCallback) {
            stateChangeCallback(GameState::WIN);
        }


    } catch (json::parse_error &e) {
        std::cerr << "Received an invalid JSON message: " << message << " error:" << e.what() << endl;
    }
}

void Client::updateLocalLobbyState(const string &message) {
    json json = json::parse(message);

    m_lobbyState.players.clear();
    for (const auto &playerJson: json["players"]) {
        LobbyState::PlayerInfo player;
        player.name = playerJson["name"];
        player.isReady = playerJson["isReady"];
        m_lobbyState.players.push_back(player);
    }
}

void Client::updateLocalGameplayState(const string &message) {
    json json = json::parse(message);

    if (!m_isGameStateInitialized) {
        m_gameplayState.initializeFromJson(json);
        m_isGameStateInitialized = true;
    }

    for (int i = 0; i < 2; i++) {
        const auto &playerJson = json["players"][i];
        m_gameplayState.getPlayers()[i]->setTurn(playerJson["onTurn"]);
        m_gameplayState.getPlayers()[i]->setHp(playerJson["hp"]);
        m_gameplayState.getPlayers()[i]->setMana(playerJson["mana"]);
        m_gameplayState.getPlayers()[i]->setDeck(deserialize<Deck>(playerJson["deck"]));
        m_gameplayState.getPlayers()[i]->setHand(deserialize<CardContainer<5>>(playerJson["hand"]));
        m_gameplayState.getPlayers()[i]->setBoard(deserialize<CardContainer<5>>(playerJson["board"]));
    }
}

void Client::shutdown() {
    if (m_socket >= 0) {
        ::shutdown(m_socket, SHUT_RDWR);
        close(m_socket);
        m_socket = -1;
    }

    if (m_serverListener.joinable()) {
        m_serverListener.join();
    }

    m_isGameStateInitialized = false;
}

int Client::getSocket() const {
    return m_socket;
}

int Client::getID() const {
    return m_ID;
}

LobbyState Client::getLobbyState() const {
    return m_lobbyState;
}

Game &Client::getGameplayState() {
    return m_gameplayState;
}

bool Client::isGameStateInitialized() const {
    return m_isGameStateInitialized;
}
