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

void Client::listenToServer() {
    while (true) {
        auto message = protocol::recvFrame(m_socket);
        if (!message.has_value()) {
            std::cerr << "Connection to server closed." << std::endl;
            break;
        }

        processMessage(*message);
    }
}

void Client::sendMessage(const string &message, const json &data) const {
    json j;
    j["type"] = message;
    j["data"] = data;
    string serializedMsg = j.dump();

//    cout << "Client sending message: " << j.dump(4) << endl;

    if (!protocol::sendFrame(m_socket, serializedMsg)) {
        std::cerr << "Failed to send message: " << message << std::endl;
    }
}

void Client::setStateChangeCallback(const StateChangeCallback &callback) {
    stateChangeCallback = callback;
}

void Client::processMessage(const string &message) {
    try {
        json j = json::parse(message);

        string type = j["type"];
        string data = j["data"].dump();

//        cout << "Client received message = " << j.dump(4) << endl;

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

        if (type == "endGame" && stateChangeCallback) {
            stateChangeCallback(GameState::END);
        }


    } catch (json::parse_error &e) {
        std::cerr << "Received an invalid JSON message: " << message << " error:" << e.what() << endl;
    } catch (std::exception &e) {
        std::cerr << "Failed to process message: " << message << " error:" << e.what() << endl;
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

    if (!json["selectedCard"].is_null()) {
        Card selectedCard = Card::createFromJson(json["selectedCard"]);
        m_gameplayState.setSelectedCard(selectedCard);
    } else {
        m_gameplayState.deselectCard();
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
