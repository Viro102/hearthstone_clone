#include <Deck.h>

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
        std::cerr << "Error: " << e.what() << endl;
    }

    return *this;
}

void Deck::shuffleDeck() {
    std::random_device rd;
    std::mt19937 generator(rd());

    std::ranges::shuffle(m_cards, generator);
}
