#include <Deck.h>

Deck::Deck(const string &filename) {
    makeDeck(filename);
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

int Deck::getMaxCards() const {
    return 30;
}
