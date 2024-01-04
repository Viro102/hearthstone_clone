#include "../../include/Deck.h"

Deck::Deck(const string &filename) {
    makeDeck(filename);
}

void Deck::addCard(const Card &card) {
    m_numberOfCards++;
    m_cards.push_back(card);
}

void Deck::removeCard(const Card &card) {
    m_numberOfCards--;
    std::erase(m_cards, card);
}

vector<Card> &Deck::getCards() {
    return m_cards;
}

Card &Deck::getCard(int i) {
    return m_cards[i];
}

int Deck::getNumOfCards() const {
    return m_numberOfCards;
}

string Deck::getNumOfCardsString() const {
    return std::to_string(m_numberOfCards);
}

Deck &Deck::makeDeck(const string &filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file: " + filename);
        }

        string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);

            string name;
            string type;
            int buffType = 0;
            int hp;
            int damage;
            int cost;

            ss >> name >> type;

            if (type == "buff") {
                ss >> buffType;
            }

            ss >> hp >> damage >> cost;

            if (buffType == 0) {
                addCard(Card(name, type, hp, damage, cost));
            } else {
                addCard(Card(name, type, buffType, hp, damage, cost));
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return *this;
}
