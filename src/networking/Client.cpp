#include <Client.h>

Client::Client(int socketFD) : m_socket(socketFD) {}

Client::~Client() {
    shutdown();
}

int Client::start(short port) {
    struct sockaddr_in serverAddress{};

    if ((m_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "\n Socket creation error \n";
        return -1;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    if (inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0) {
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
        char buffer[1024] = {0};
        auto valread = recv(m_socket, buffer, 1024, 0);
        if (valread > 0) {
            string message(buffer, valread);
            cout << "Client: received message = " << message << endl;
            processMessage(message);
        } else {
            break;
        }
    }
}


void Client::sendMessage(const string &message) const {
    json j;
    j["type"] = message;
    j["data"] = "";
    string serializedMsg = j.dump();

    send(m_socket, serializedMsg.c_str(), serializedMsg.size(), 0);
}

void Client::setStateChangeCallback(const StateChangeCallback &callback) {
    stateChangeCallback = callback;
}

void Client::processMessage(const string &message) {
    try {
        json j = json::parse(message);

        // Extract the message type
        string type = j["type"];
        string data = j["data"].dump();

        if (type == "updateLobbyState") {
            updateLocalLobbyState(data);
        }

        if (type == "startGame" && stateChangeCallback) {
            stateChangeCallback(GameState::GAMEPLAY);
        }

        if (type == "updateGameState") {
            updateLocalGameplayState(data);
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

    for (int i = 0; i < json["players"].size(); i++) {
        const auto &playerJson = json["players"][i];
        m_gameplayState.getPlayers()[i]->setHp(playerJson["hp"]);
        m_gameplayState.getPlayers()[i]->setMana(playerJson["mana"]);
        m_gameplayState.getPlayers()[i]->setDeck(deserializeDeck(playerJson["deck"]));
        m_gameplayState.getPlayers()[i]->setHand(deserializeContainer(playerJson["hand"]));
        m_gameplayState.getPlayers()[i]->setBoard(deserializeContainer(playerJson["board"]));
    }
}

std::unique_ptr<Deck> Client::deserializeDeck(const json &jsonArray) {
    auto deck = std::make_unique<Deck>();
    for (const auto &cardJson: jsonArray) {
        Card newCard = Card::createFromJson(cardJson);
        deck->addCard(newCard);
    }
    return deck;
}

std::unique_ptr<CardContainer<5>> Client::deserializeContainer(const json &jsonArray) {
    auto cardContainer = std::make_unique<CardContainer<5>>();
    for (const auto &cardJson: jsonArray) {
        Card newCard = Card::createFromJson(cardJson);
        cardContainer->addCard(newCard);
    }
    return cardContainer;
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
}

int Client::getSocket() const {
    return m_socket;
}

LobbyState Client::getLobbyState() const {
    return m_lobbyState;
}

Game &Client::getGameplayState() {
    return m_gameplayState;
}
