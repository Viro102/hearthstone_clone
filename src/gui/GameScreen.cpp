#include <GameScreen.h>

GameScreen::GameScreen(Game &game) : m_game(game) {
    m_images.resize(4);

    for (int i = 0; i < m_slotsHand.size(); i++) {
        m_slotsHand[i] = Slot(30 + (i * 170), 530);
    }

    for (int i = 0; i < m_slotsBoard.size(); i++) {
        for (int j = 0; j < m_slotsBoard[i].size(); j++) {
            int x = std::abs(i - 1);
            m_slotsBoard[x][j] = Slot(30 + (j * 170), 40 + (i * 240));
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

    m_heroHitbox = Rectangle(m_images[2].width, m_images[2].height);
    m_heroHitbox.x = HEROES_POSITION_X;
    m_heroHitbox.y = SECOND_HERO_POSITION_Y;

    m_endTurnButtonHitbox = Rectangle(200, 100);
    m_endTurnButtonHitbox.x = END_TURN_BUTTON_POSITION_X;
    m_endTurnButtonHitbox.y = END_TURN_BUTTON_POSITION_Y;

    registerAsClickable("heroAvatar", m_heroHitbox);
    registerAsClickable("endTurnButton", m_endTurnButtonHitbox);
}

GameScreen::~GameScreen() {
    for (const auto &image: m_images) {
        UnloadTexture(image);
    }
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
    const auto &currentPlayer = m_game.getPlayers()[0];
    const auto &opponentPlayer = m_game.getPlayers()[1];

    // End turn button
    DrawText("END TURN", END_TURN_BUTTON_POSITION_X, END_TURN_BUTTON_POSITION_Y, 20, BLACK);
    DrawRectangle(END_TURN_BUTTON_POSITION_X, END_TURN_BUTTON_POSITION_Y, 200, 70, GRAY);

    if (currentPlayer->getArchetype().empty()) {
        return;
    }

    // Draw heroes
    if (currentPlayer->getArchetype() == "mage") {
        paintHero(0, mage, *currentPlayer);
        paintHero(1, warrior, *opponentPlayer);
    } else if (currentPlayer->getArchetype() == "warrior") {
        paintHero(0, warrior, *currentPlayer);
        paintHero(1, mage, *opponentPlayer);
    }

    DrawTexture(board, 20, 10, WHITE);
    DrawTexture(deck, 1150, 200, WHITE);
    DrawText(("Cards: " + currentPlayer->getDeck().getNumOfCardsString()).c_str(), 1170, 200, 20, BLACK);
}

void GameScreen::paintCards(const vector<Card> &cards) {
    for (const auto &card: cards) {
        int attPosY = card.getY() + card.getHeight();
        int attPosX = card.getX() + 2;

        int hpPosY = card.getY() + card.getHeight();
        int hpPosX = card.getX() + card.getWidth() - 18;

        int costPosY = card.getY() + 25;
        int costPosX = card.getX() + 2;

        int namePosY = card.getY() + 100;
        int namePosX = card.getX() + 40;

        int typePosY = card.getY() + 125;
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
    const auto &players = m_game.getPlayers();
    const auto &currentPlayer = m_game.getOnTurnPlayer();
    auto &playerCardsHand = currentPlayer.getHand().getCards();

    for (size_t i = 0; i < playerCardsHand.size(); i++) {
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

        for (size_t i = 0; i < playerCardsBoard.size(); i++) {
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
    // Clear the background
    ClearBackground(RAYWHITE);

    // Draw the UI elements
    paintUI();

    // Combine the hand and board cards into a single list for painting
    vector<Card> allCards;
    allCards.insert(allCards.end(), m_cardsHand.begin(), m_cardsHand.end());
    allCards.insert(allCards.end(), m_cardsBoard.begin(), m_cardsBoard.end());

    // Draw the cards
    paintCards(allCards);

    // Draw glows for slots on the board
    for (size_t i = 0; i < m_slotsBoard.size(); i++) {
        for (size_t j = 0; j < m_slotsBoard[i].size(); j++) {
            if (!m_slotsBoard[i][j].isFree() && m_slotsBoard[i][j].isGlow()) {
                DrawRectangleLinesEx(m_slotsBoard[i][j].getShape(), 2, YELLOW); // Assuming getShape returns a Rectangle
            }
        }
    }

    // Draw glows for slots in hand
    for (const auto &slot: m_slotsHand) {
        if (!slot.isFree() && slot.isGlow()) {
            DrawRectangleLinesEx(slot.getShape(), 2, YELLOW); // Assuming getShape returns a Rectangle
        }
    }
}

void GameScreen::addGlow(int i, const string &where) {
    const auto &currentPlayer = m_game.getOnTurnPlayer();
    if (where == "hand") {
        m_slotsHand[i].setGlow(true);
    } else if (where == "board") {
        m_slotsBoard[currentPlayer.getId()][i].setGlow(true);
    }
}

void GameScreen::removeGlow() {
    for (int i = 0; i < m_slotsBoard.size(); i++) {
        for (int j = 0; j < m_slotsBoard[i].size(); j++) {
            m_slotsBoard[i][j].setGlow(false);
        }
    }

    for (auto &i: m_slotsHand) {
        i.setGlow(false);
    }
}

void GameScreen::registerAsClickable(const string &name, const Rectangle &hitbox) {
    m_clickableObjects[name] = hitbox;
}

const std::map<string, Rectangle> &GameScreen::getClickableObjects() const {
    return m_clickableObjects;
}
