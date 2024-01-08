#include <Card.h>

Card::Card(string name, string type, int buffAmount, int hp, int damage, int cost)
        : m_name(std::move(name)), m_type(std::move(type)),
          m_buffAmount(buffAmount), m_hp(hp), m_damage(damage), m_cost(cost) {}

Card::Card(string name, string type, int hp, int damage, int cost)
        : m_name(std::move(name)), m_type(std::move(type)),
          m_hp(hp), m_damage(damage), m_cost(cost) {}

Card Card::createFromJson(const json &jsonData) {
    return {
            jsonData["name"],
            jsonData["type"],
            jsonData["buffAmount"],
            jsonData["hp"],
            jsonData["damage"],
            jsonData["cost"]
    };
}

json Card::serialize() const {
    return {
            {"name",        m_name},
            {"type",        m_type},
            {"buffAmount",  m_buffAmount},
            {"hp",          m_hp},
            {"damage",      m_damage},
            {"cost",        m_cost},
            {"x",           m_x},
            {"y",           m_y},
            {"width",       m_width},
            {"height",      m_height},
            {"hasAttacked", m_hasAttacked}
    };
}

void Card::print() const {
    cout << "Card Information:" << endl;
    cout << "Name: " << m_name << endl;
    cout << "Type: " << m_type << endl;
    cout << "Buff Amount: " << m_buffAmount << endl;
    cout << "HP: " << m_hp << endl;
    cout << "Damage: " << m_damage << endl;
    cout << "Cost: " << m_cost << endl;
    cout << "X Position: " << m_x << endl;
    cout << "Y Position: " << m_y << endl;
    cout << "Width: " << m_width << endl;
    cout << "Height: " << m_height << endl;
    cout << "Has Attacked: " << (m_hasAttacked ? "Yes" : "No") << endl;
}

const string &Card::getName() const {
    return m_name;
}

const string &Card::getType() const {
    return m_type;
}

int Card::getHp() const {
    return m_hp;
}

int Card::getDamage() const {
    return m_damage;
}

int Card::getCost() const {
    return m_cost;
}

int Card::getBuffAmount() const {
    return m_buffAmount;
}

int Card::getX() const {
    return m_x;
}

int Card::getY() const {
    return m_y;
}

int Card::getHeight() const {
    return m_height;
}

int Card::getWidth() const {
    return m_width;
}

bool Card::getHasAttacked() const {
    return m_hasAttacked;
}

void Card::setHasAttacked(bool hasAttacked) {
    m_hasAttacked = hasAttacked;
}

void Card::setHp(int hp) {
    m_hp = hp;
}

void Card::setDamage(int damage) {
    m_damage = damage;
}

void Card::setX(int x) {
    m_x = x;
}

void Card::setY(int y) {
    m_y = y;
}

void Card::setPosition(Rectangle rec) {
    setX(static_cast<int>(rec.x));
    setY(static_cast<int>(rec.y));
}
