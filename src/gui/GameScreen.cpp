#include <GameScreen.h>

GameScreen::GameScreen(Game &gameplayState) : m_gameplayState(gameplayState) {
    m_images.resize(4);

    for (int i = 0; i < m_slotsHand.size(); i++) {
        auto newSlot = Slot(30 + (i * 170), 530);
        m_slotsHand[i] = Slot(30 + (i * 170), 530);
        string label = "slotHand[" + std::to_string(i) + "]";
        registerAsClickable(label, newSlot.getShape());
    }

    for (int i = 0; i < m_slotsBoard.size(); i++) {
        for (int j = 0; j < m_slotsBoard[i].size(); j++) {
            int x = std::abs(i - 1);
            auto newSlot = Slot(30 + (j * 170), 40 + (i * 240));
            m_slotsBoard[x][j] = newSlot;
            string label = "slotBoard[" + std::to_string(x) + "][" + std::to_string(j) + "]";
            registerAsClickable(label, newSlot.getShape());
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

    m_heroHitboxPlayer = Rectangle(HEROES_POSITION_X, FIRST_HERO_POSITION_Y, m_images[3].width, m_images[3].height);
    m_heroHitboxOpponent = Rectangle(HEROES_POSITION_X, SECOND_HERO_POSITION_Y, m_images[2].width, m_images[2].height);

    m_endTurnButtonHitbox = Rectangle(END_TURN_BUTTON_POSITION_X, END_TURN_BUTTON_POSITION_Y, 200, 100);

    registerAsClickable("heroPlayer", m_heroHitboxPlayer);
    registerAsClickable("heroOpponent", m_heroHitboxOpponent);
    registerAsClickable("endTurnButton", m_endTurnButtonHitbox);
}

void GameScreen::paintHero(int pos, Texture2D hero, const Player &player) const {
    const int OFFSET_X = 220;
    const int OFFSET_Y_HP = 650;
    const int OFFSET_Y_MANA = 700;
    const int FONT_SIZE = 20;

    if (pos == 0) {
        DrawTexture(hero, HEROES_POSITION_X, FIRST_HERO_POSITION_Y, WHITE);
        DrawText(("HP: " + player.getHpString()).c_str(), HEROES_POSITION_X + OFFSET_X, OFFSET_Y_HP, FONT_SIZE, BLACK);
        DrawText(("MANA: " + player.getManaString()).c_str(), HEROES_POSITION_X + OFFSET_X, OFFSET_Y_MANA, FONT_SIZE,
                 BLACK);
    } else {
        DrawTexture(hero, HEROES_POSITION_X, SECOND_HERO_POSITION_Y, WHITE);
        DrawText(("HP: " + player.getHpString()).c_str(), HEROES_POSITION_X + OFFSET_X, 50, FONT_SIZE, BLACK);
        DrawText(("MANA: " + player.getManaString()).c_str(), HEROES_POSITION_X + OFFSET_X, 100, FONT_SIZE, BLACK);
    }
}

void GameScreen::paintUI() const {
    const auto &board = m_images[0];
    const auto &deck = m_images[1];
    const auto &mage = m_images[2];
    const auto &warrior = m_images[3];
    const auto &currentPlayer = m_gameplayState.getOnTurnPlayer();
    const auto &opponentPlayer = m_gameplayState.getOffTurnPlayer();

    // End turn button
    int endTurnWidth = MeasureText("END TURN", 20);
    DrawRectangle(END_TURN_BUTTON_POSITION_X, END_TURN_BUTTON_POSITION_Y, 200, 70, GRAY);
    DrawText("END TURN", END_TURN_BUTTON_POSITION_X + 200 / 2 - endTurnWidth / 2,
             END_TURN_BUTTON_POSITION_Y + 70 / 2, 20, BLACK);

    if (currentPlayer.getArchetype().empty()) {
        return;
    }

    // Draw heroes
    if (currentPlayer.getArchetype() == "mage") {
        paintHero(0, mage, currentPlayer);
        paintHero(1, warrior, opponentPlayer);
    } else if (currentPlayer.getArchetype() == "warrior") {
        paintHero(0, warrior, currentPlayer);
        paintHero(1, mage, opponentPlayer);
    }

    DrawTexture(board, 20, 10, WHITE);
    DrawTexture(deck, 1150, 200, WHITE);
    DrawText(("Cards: " + currentPlayer.getDeck().getNumOfCardsString()).c_str(), 1170, 175, 20, BLACK);
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
            m_slotsHand[i].setFree(false);
            auto shape = m_slotsHand[i].getShape();
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
                m_slotsBoard[id][i].setFree(false);
                auto shape = m_slotsBoard[id][i].getShape();
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

    // Draw glow for slots on the board
    for (int i = 0; i < m_slotsBoard.size(); i++) {
        for (int j = 0; j < m_slotsBoard[i].size(); j++) {
            if (!m_slotsBoard[i][j].isFree() && m_slotsBoard[i][j].isGlow()) {
                DrawRectangleLinesEx(m_slotsBoard[i][j].getShape(), 2, YELLOW);
            }
        }
    }

    // Draw glow for slots in hand
    for (const auto &slot: m_slotsHand) {
        if (!slot.isFree() && slot.isGlow()) {
            DrawRectangleLinesEx(slot.getShape(), 2, YELLOW);
        }
    }
}

void GameScreen::addGlow(int i) {
    const auto &currentPlayer = m_gameplayState.getOnTurnPlayer();
    m_slotsBoard[currentPlayer.getId()][i].setGlow(true);
}

void GameScreen::removeGlow() {
    for (int i = 0; i < m_slotsBoard.size(); i++) {
        for (int j = 0; j < m_slotsBoard[i].size(); j++) {
            m_slotsBoard[i][j].setGlow(false);
        }
    }
}

void GameScreen::registerAsClickable(const string &name, const Rectangle &hitbox) {
    m_clickableObjects[name] = hitbox;
}

const std::map<string, Rectangle> &GameScreen::getClickableObjects() const {
    return m_clickableObjects;
}
