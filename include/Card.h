#pragma once

#include <Common.h>
#include <raylib.h>

class Card {
public:
    Card(string name, string type, int buffAmount, int hp, int damage, int cost);

    Card(string name, string type, int hp, int damage, int cost);

    Card() = default;

    Card(const Card &other) = default;

    static Card createFromJson(const json &jsonData);

    [[nodiscard]] json serialize() const;

    [[nodiscard]] const string &getName() const;

    [[nodiscard]] const string &getType() const;

    [[nodiscard]] int getHp() const;

    [[nodiscard]] int getDamage() const;

    [[nodiscard]] int getCost() const;

    [[nodiscard]] int getBuffAmount() const;

    [[nodiscard]] int getX() const;

    [[nodiscard]] int getY() const;

    [[nodiscard]] int getHeight() const;

    [[nodiscard]] int getWidth() const;

    [[nodiscard]] bool getHasAttacked() const;

    void setHasAttacked(bool hasAttacked);

    void setHp(int hp);

    void setDamage(int damage);

    void setX(int x);

    void setY(int y);

    void setPosition(Rectangle rec);

    void print() const;

    bool operator==(const Card &other) const;


private:
    string m_name;
    string m_type;
    int m_buffAmount{};
    int m_hp{};
    int m_damage{};
    int m_cost{};
    int m_x{};
    int m_y{};
    int m_width{150};
    int m_height{200};
    bool m_hasAttacked{};
};
