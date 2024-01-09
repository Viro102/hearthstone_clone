#include <GameScreen.h>

GameScreen::GameScreen(Client &client) : m_client(client) {
    m_images.resize(4);

    for (int i = 0; i < m_slotsHand.size(); i++) {
        auto newSlot = Button(30 + (i * 170), 530);
        m_slotsHand[i] = newSlot;
    }

    for (int i = 0; i < m_slotsBoard.size(); i++) {
        for (int j = 0; j < m_slotsBoard[i].size(); j++) {
            auto newSlot = Button(30 + (j * 170), 40 + (i * 240));
            m_slotsBoard[i][j] = newSlot;
        }
    }


    auto board = LoadImage("../assets/board.png");
    auto deck = LoadImage("../assets/deck.png");
    auto mage = LoadImage("../assets/mage.png");
    auto warrior = LoadImage("../assets/warrior.png");

    m_images[0] = LoadTextureFromImage(board);
    m_images[1] = LoadTextureFromImage(deck);
    m_images[2] = LoadTextureFromImage(mage);
    m_images[3] = LoadTextureFromImage(warrior);

    UnloadImage(board);
    UnloadImage(deck);
    UnloadImage(mage);
    UnloadImage(warrior);

    int i = m_client.getGameplayState().getPlayer(m_client.getID()).getArchetype() == "mage" ? 2 : 3;
    int other = i == 2 ? 3 : 2;

    auto heroPlayer = Button(HEROES_POSITION_X, FIRST_HERO_POSITION_Y, m_images[i]);
    auto heroOpponent = Button(HEROES_POSITION_X, SECOND_HERO_POSITION_Y, m_images[other]);
    auto endTurn = Button(Rectangle(END_TURN_BUTTON_POSITION_X, END_TURN_BUTTON_POSITION_Y, 200, 75), "END TURN");

    m_buttons["heroPlayer"] = heroPlayer;
    m_buttons["heroOpponent"] = heroOpponent;
    m_buttons["endTurn"] = endTurn;
}

void GameScreen::paintHero(const Player *player, int posYHero, int offsetYHP, int offsetYMana,
                           const std::string &buttonKey) {
    const int OFFSET_X = 220;
    const int FONT_SIZE = 20;

    m_buttons.at(buttonKey).setPosition(HEROES_POSITION_X, posYHero);
    m_buttons.at(buttonKey).draw();
    DrawText(("HP: " + player->getHpString()).c_str(), HEROES_POSITION_X + OFFSET_X, offsetYHP, FONT_SIZE, BLACK);
    DrawText(("MANA: " + player->getManaString()).c_str(), HEROES_POSITION_X + OFFSET_X, offsetYMana, FONT_SIZE, BLACK);
}


void GameScreen::paintUI() {
    const auto &board = m_images[0];
    const auto &deck = m_images[1];
    const Player *currentPlayer{};
    const Player *opponentPlayer{};

    for (const auto &player: m_client.getGameplayState().getPlayers()) {
        if (player->getId() == m_client.getID()) {
            currentPlayer = player.get();
        } else {
            opponentPlayer = player.get();
        }
    }

    if (m_client.getGameplayState().getOnTurnPlayer().getId() == m_client.getID()) {
        m_buttons["endTurn"].setColor(GREEN);
    } else {
        m_buttons["endTurn"].setColor(RED);
    }

    // End turn button
    m_buttons.at("endTurn").draw();

    // Draw heroes
    if (currentPlayer) {
        paintHero(currentPlayer, FIRST_HERO_POSITION_Y, 650, 700, "heroPlayer");
    }
    if (opponentPlayer) {
        paintHero(opponentPlayer, SECOND_HERO_POSITION_Y, 50, 100, "heroOpponent");
    }

    DrawTexture(board, 20, 10, WHITE);
    DrawTexture(deck, 1150, 200, WHITE);
    if (currentPlayer) {
        DrawText(("Cards: " + currentPlayer->getDeck().getNumOfCardsString()).c_str(), 1170, 175, 20, BLACK);
    }
}


void GameScreen::paintCards(const vector<Card> &cards) {
    for (const auto &card: cards) {
        if (!card.getName().empty()) {
            int attPosY = card.getY() + card.getHeight() - 28;
            int attPosX = card.getX() + 2;

            int hpPosY = card.getY() + card.getHeight() - 28;
            int hpPosX = card.getX() + card.getWidth() - 18;

            int costPosY = card.getY();
            int costPosX = card.getX() + 2;

            int namePosY = card.getY() + 80;
            int namePosX = card.getX() + 40;

            int typePosY = card.getY() + 100;
            int typePosX = card.getX() + 45;

            // Draw card rectangle
            DrawRectangle(card.getX(), card.getY(), card.getWidth(), card.getHeight(), PINK);

            // Draw HP, damage, and cost
            DrawText(std::to_string(card.getHp()).c_str(), hpPosX, hpPosY, 30, BLACK);
            DrawText(std::to_string(card.getDamage()).c_str(), attPosX, attPosY, 30, BLACK);
            DrawText(std::to_string(card.getCost()).c_str(), costPosX, costPosY, 30, BLACK);

            // Draw name and type
            DrawText(card.getName().c_str(), namePosX, namePosY, 20, BLACK);
            DrawText(card.getType().c_str(), typePosX, typePosY, 17, BLACK);
        }
    }
}

void GameScreen::update() {
    m_cardsHand.clear();
    m_cardsBoard.clear();
    const auto &players = m_client.getGameplayState().getPlayers();
    const auto &playerCardsHand = m_client.getGameplayState().getPlayer(m_client.getID()).getHand().getCards();

    for (int i = 0; i < playerCardsHand.size(); i++) {
        const auto &card = playerCardsHand[i];
        if (card != nullptr) {
            m_slotsHand[i].setFree(false);
            auto shape = m_slotsHand[i].getHitbox();
            card->setPosition(shape);
            m_cardsHand.push_back(*card);

            if (m_slotsHand[i].isClicked() && m_client.getGameplayState().getPlayer(m_client.getID()).isTurn()) {
                json j = {{"index", i}};
                m_client.sendMessage("playCard", j);
            }
        } else {
            m_slotsHand[i].setFree(true);
        }
    }

    for (const auto &player: players) {
        int row = player->getId() == m_client.getID() ? 0 : 1;
        auto &playerCardsBoard = player->getBoard().getCards();
        for (int i = 0; i < playerCardsBoard.size(); i++) {
            if (playerCardsBoard[i] != nullptr) {
                m_slotsBoard[row][i].setFree(false);
                auto shape = m_slotsBoard[row][i].getHitbox();
                playerCardsBoard[i]->setPosition(shape);
                m_cardsBoard.push_back(*playerCardsBoard[i]);

                if (row == 0 && m_slotsBoard[row][i].isClicked()) {
                    json j = {{"index", i}};
                    if (m_client.getGameplayState().isSelected()) {
                        m_client.sendMessage("attack", j);
                    } else {
                        m_client.sendMessage("selectCardBoard", j);
                    }
                }
            } else {
                m_slotsBoard[row][i].setFree(true);
            }
        }
    }

//    if (m_buttons["heroPlayer"].isClicked() || m_buttons["heroOpponent"].isClicked()) {
//
//    }

    if (m_buttons["endTurn"].isClicked() && m_client.getGameplayState().getPlayer(m_client.getID()).isTurn()) {
        m_client.sendMessage("endTurn");
    }
}

void GameScreen::draw() {
    paintUI();

    // Combine the hand and board cards into a single list for painting
    vector<Card> allCards;
    allCards.insert(allCards.end(), m_cardsHand.begin(), m_cardsHand.end());
    allCards.insert(allCards.end(), m_cardsBoard.begin(), m_cardsBoard.end());

    paintCards(allCards);

    std::ranges::for_each(m_slotsHand, [](const auto &slot) { slot.draw(); });
    std::ranges::for_each(m_slotsBoard, [](const auto &slot) {
        std::ranges::for_each(slot, [](const auto &element) { element.draw(); });
    });

    std::ranges::for_each(m_buttons, [](const auto &pair) { pair.second.draw(); });
}

void GameScreen::print() {
    cout << "---- GameScreen State ----" << endl;

    cout << "Game State: " << endl;

    m_client.getGameplayState().print();

    cout << "Screen cards in Hand: " << m_cardsHand.size() << endl;
    for (const auto &card: m_cardsHand) {
        cout << "Card: " << card.getName() << " | HP: " << card.getHp() << " | Damage: " << card.getDamage() << endl;
    }

    cout << "Screen cards on Board: " << m_cardsBoard.size() << endl;
    for (const auto &card: m_cardsBoard) {
        cout << "Card: " << card.getName() << " | HP: " << card.getHp() << " | Damage: " << card.getDamage() << endl;
    }

    cout << "-------------------------" << endl;
}
