#include <GameScreen.h>

GameScreen::GameScreen(Game &gameplayState) : m_gameplayState(gameplayState) {
    m_images.resize(4);

    for (int i = 0; i < m_slotsHand.size(); i++) {
        auto newSlot = Button(30 + (i * 170), 530);
        m_slotsHand[i] = newSlot;
        string label = "slotHand[" + std::to_string(i) + "]";
        m_buttons[label] = newSlot;
    }

    for (int i = 0; i < m_slotsBoard.size(); i++) {
        for (int j = 0; j < m_slotsBoard[i].size(); j++) {
            int x = std::abs(i - 1);
            auto newSlot = Button(30 + (j * 170), 40 + (i * 240));
            m_slotsBoard[x][j] = newSlot;
            string label = "slotBoard[" + std::to_string(x) + "][" + std::to_string(j) + "]";
            m_buttons[label] = newSlot;
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

    m_buttons["heroPlayer"] = Button(HEROES_POSITION_X, FIRST_HERO_POSITION_Y, m_images[3]);
    m_buttons["heroOpponent"] = Button(HEROES_POSITION_X, SECOND_HERO_POSITION_Y, m_images[2]);
    m_buttons["endTurn"] = Button(Rectangle(END_TURN_BUTTON_POSITION_X, END_TURN_BUTTON_POSITION_Y,
                                            200, 75), "END TURN");
}

void GameScreen::paintHero(int pos, const Player &player) const {
    const int OFFSET_X = 220;
    const int OFFSET_Y_HP = 650;
    const int OFFSET_Y_MANA = 700;
    const int FONT_SIZE = 20;

    if (pos == 0) {
        m_buttons.at("heroPlayer").draw();
        DrawText(("HP: " + player.getHpString()).c_str(), HEROES_POSITION_X + OFFSET_X, OFFSET_Y_HP, FONT_SIZE, BLACK);
        DrawText(("MANA: " + player.getManaString()).c_str(), HEROES_POSITION_X + OFFSET_X, OFFSET_Y_MANA, FONT_SIZE,
                 BLACK);
    } else {
        m_buttons.at("heroOpponent").draw();
        DrawText(("HP: " + player.getHpString()).c_str(), HEROES_POSITION_X + OFFSET_X, 50, FONT_SIZE, BLACK);
        DrawText(("MANA: " + player.getManaString()).c_str(), HEROES_POSITION_X + OFFSET_X, 100, FONT_SIZE, BLACK);
    }
}

void GameScreen::paintUI() const {
    const auto &board = m_images[0];
    const auto &deck = m_images[1];
    const auto &currentPlayer = m_gameplayState.getPlayers()[0];
    const auto &opponentPlayer = m_gameplayState.getPlayers()[1];

    // End turn button
    m_buttons.at("endTurn").draw();

    if (currentPlayer->getArchetype().empty()) {
        return;
    }

    // Draw heroes
    paintHero(0, *currentPlayer);
    paintHero(1, *opponentPlayer);

    DrawTexture(board, 20, 10, WHITE);
    DrawTexture(deck, 1150, 200, WHITE);
    DrawText(("Cards: " + currentPlayer->getDeck().getNumOfCardsString()).c_str(), 1170, 175, 20, BLACK);
}

void GameScreen::paintCards(const vector<Card> &cards) {
    for (const auto &card: cards) {
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

void GameScreen::update() {
    m_cardsHand.clear();
    m_cardsBoard.clear();
    const auto &players = m_gameplayState.getPlayers();
    const auto &currentPlayer = m_gameplayState.getOnTurnPlayer();
    auto &playerCardsHand = currentPlayer.getHand().getCards();

    for (int i = 0; i < playerCardsHand.size(); i++) {
        const auto &cardToPaint = playerCardsHand[i];
        if (cardToPaint != nullptr) {
            m_slotsHand[i].update();
            m_slotsHand[i].setFree(false);
            auto shape = m_slotsHand[i].getHitbox();
            cardToPaint->setPosition(shape);
            m_cardsHand.push_back(*cardToPaint);
        } else {
            m_slotsHand[i].setFree(true);
        }
    }

    for (const auto &player: players) {
        int id = player->getId();
        auto &playerCardsBoard = player->getBoard().getCards();

        for (int i = 0; i < playerCardsBoard.size(); i++) {
            if (playerCardsBoard[i] != nullptr) {
                m_slotsBoard[id][i].update();
                m_slotsBoard[id][i].setFree(false);
                auto shape = m_slotsBoard[id][i].getHitbox();
                playerCardsBoard[i]->setPosition(shape);
                m_cardsBoard.push_back(*playerCardsBoard[i]);
            } else {
                m_slotsBoard[id][i].setFree(true);
            }
        }
    }
}

void GameScreen::draw() {
    ClearBackground(RAYWHITE);

    paintUI();

    // Combine the hand and board cards into a single list for painting
    vector<Card> allCards;
    allCards.insert(allCards.end(), m_cardsHand.begin(), m_cardsHand.end());
    allCards.insert(allCards.end(), m_cardsBoard.begin(), m_cardsBoard.end());

    paintCards(allCards);

    std::ranges::for_each(m_buttons, [](const auto &pair) {
        pair.second.draw();
    });
}
