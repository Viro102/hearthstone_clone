#include <Deck.h>

Deck &Deck::makeDeck(const string &filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file: " + filename);
        }

        string line;
        int lineNumber = 0;
        while (std::getline(file, line)) {
            lineNumber++;
            if (line.find_first_not_of(" \t\r") == string::npos) {
                continue;
            }
            std::stringstream ss(line);

            string name;
            string type;
            int buffAmount = 0;
            int hp = 0;
            int damage = 0;
            int cost = 0;

            ss >> name >> type;

            if (type == "buff") {
                ss >> buffAmount;
            }

            ss >> hp >> damage >> cost;

            if (ss.fail()) {
                std::cerr << "Skipping malformed card on line " << lineNumber << " of " << filename << ": "
                          << line << endl;
                continue;
            }

            addCard(Card(name, type, buffAmount, hp, damage, cost));
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
