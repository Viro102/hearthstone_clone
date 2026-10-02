#include <Client.h>

Client::Client(int socketFD) : m_socket(socketFD) {}

Client::~Client() {
    shutdown();
}

int Client::start(uint16_t port, const string &ipAddr) {
    shutdown();

    struct sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    if (inet_pton(AF_INET, ipAddr.c_str(), &serverAddress.sin_addr) <= 0) {
        cout << "\nInvalid address/Address not supported\n";
        return -1;
    }

    int socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0) {
        cout << "\n Socket creation error \n";
        return -1;
    }

    if (connect(socketFD, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        cout << "\nConnection Failed\n";
        close(socketFD);
        return -1;
    }

    {
        std::scoped_lock lock(m_stateMutex);
        m_ID = -1;
        m_lobbyState = {};
        m_isGameStateInitialized = false;
        m_pendingState.reset();
        m_notice.clear();
    }

    m_socket = socketFD;
    m_isShuttingDown = false;
    m_serverListener = std::jthread(&Client::listenToServer, this, socketFD);
    return 0;
}

void Client::listenToServer(int socket) {
    while (true) {
        auto message = protocol::recvFrame(socket);
        if (!message.has_value()) {
            break;
        }
        processMessage(*message);
    }

    if (!m_isShuttingDown) {
        std::cerr << "Connection to server closed." << std::endl;
        std::scoped_lock lock(m_stateMutex);
        if (m_notice.empty()) {
            m_notice = "Disconnected from server";
        }
        m_pendingState = GameState::MENU;
    }
}

void Client::sendMessage(const string &message, const json &data) const {
    json j;
    j["type"] = message;
    j["data"] = data;

    if (!protocol::sendFrame(m_socket, j.dump())) {
        std::cerr << "Failed to send message: " << message << std::endl;
    }
}

void Client::processMessage(const string &message) {
    try {
        json j = json::parse(message);

        string type = j["type"];
        json data = j["data"];

        std::scoped_lock lock(m_stateMutex);

        if (type == "updateLobbyState") {
            updateLocalLobbyState(data);
        } else if (type == "yourID") {
            m_ID = data;
        } else if (type == "startGame") {
            m_isGameStateInitialized = false;
            updateLocalGameplayState(data);
            m_pendingState = GameState::GAMEPLAY;
        } else if (type == "updateGameState") {
            updateLocalGameplayState(data);
        } else if (type == "opponentDisconnected") {
            m_isGameStateInitialized = false;
            m_pendingState = GameState::WIN;
        } else if (type == "endGame") {
            m_isGameStateInitialized = false;
            m_pendingState = data["winner"] == m_ID ? GameState::WIN : GameState::LOSE;
        } else if (type == "serverFull") {
            m_notice = "Server is full";
        }

    } catch (json::parse_error &e) {
        std::cerr << "Received an invalid JSON message: " << message << " error:" << e.what() << endl;
    } catch (std::exception &e) {
        std::cerr << "Failed to process message: " << message << " error:" << e.what() << endl;
    }
}

void Client::updateLocalLobbyState(const json &data) {
    m_lobbyState.players.clear();
    for (const auto &playerJson: data["players"]) {
        LobbyState::PlayerInfo player;
        player.name = playerJson["name"];
        player.isReady = playerJson["isReady"];
        m_lobbyState.players.push_back(player);
    }
}

void Client::updateLocalGameplayState(const json &data) {
    if (!m_isGameStateInitialized) {
        m_gameplayState.initializeFromJson(data);
    }

    if (data.contains("selectedCard")) {
        Card selectedCard = Card::createFromJson(data["selectedCard"]);
        m_gameplayState.setSelectedCard(selectedCard);
    } else {
        m_gameplayState.deselectCard();
    }

    for (int i = 0; i < 2; i++) {
        const auto &playerJson = data["players"].at(i);
        auto &player = m_gameplayState.getPlayers()[i];
        player->setTurn(playerJson["onTurn"]);
        player->setHp(playerJson["hp"]);
        player->setMana(playerJson["mana"]);
        player->setDeck(deserialize<Deck>(playerJson["deck"]));
        player->setHand(deserialize<CardContainer<5>>(playerJson["hand"]));
        player->setBoard(deserialize<CardContainer<5>>(playerJson["board"]));
    }

    // Only flag the state as usable once it is complete (turns, hands and boards set).
    m_isGameStateInitialized = true;
}

void Client::shutdown() {
    m_isShuttingDown = true;
    if (m_socket >= 0) {
        ::shutdown(m_socket, SHUT_RDWR);
    }

    if (m_serverListener.joinable()) {
        m_serverListener.join();
    }

    // Close only after the listener has stopped using the socket.
    if (m_socket >= 0) {
        close(m_socket);
        m_socket = -1;
    }

    // The listener may have queued a transition (e.g. "startGame") right before it stopped;
    // after a disconnect it must not move the player out of the menu.
    std::scoped_lock lock(m_stateMutex);
    m_isGameStateInitialized = false;
    m_pendingState.reset();
}

std::unique_lock<std::mutex> Client::lockState() {
    return std::unique_lock(m_stateMutex);
}

std::optional<GameState> Client::takePendingState() {
    return std::exchange(m_pendingState, std::nullopt);
}

const string &Client::getNotice() const {
    return m_notice;
}

int Client::getSocket() const {
    return m_socket;
}

int Client::getID() const {
    return m_ID;
}

const LobbyState &Client::getLobbyState() const {
    return m_lobbyState;
}

Game &Client::getGameplayState() {
    return m_gameplayState;
}

bool Client::isGameStateInitialized() const {
    return m_isGameStateInitialized;
}
