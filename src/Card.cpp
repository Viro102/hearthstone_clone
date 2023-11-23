#include "Card.h"

#include <utility>


Card::Card(string name, string type, int buffAmount, int hp, int damage, int cost) : m_name(std::move(name)),
                                                                                     m_type(std::move(type)),
                                                                                     m_buffAmount(buffAmount), m_hp(hp),
                                                                                     m_damage(damage), m_cost(cost) {

    init();
}

Card::Card(string name, string type, int hp, int damage, int cost) : m_name(std::move(name)), m_type(std::move(type)),
                                                                     m_hp(hp),
                                                                     m_damage(damage), m_cost(cost) {

    init();
}

string Card::getName() const {
    return m_name;
}

string Card::getType() const {
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

const Rectangle &Card::getShape() const {
    return Rectangle(m_x, m_y, m_width, m_height);
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

void Card::setPosition(int x, int y) {
    setX(x);
    setY(y);
}

void Card::init() {
    m_x = 0;
    m_y = 0;
    m_width = 150;
    m_height = 200;
}

bool Card::operator==(const Card &other) const = default;
