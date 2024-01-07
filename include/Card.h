#pragma once

#include <Common.h>
#include <raylib.h>

class Card {
public:
    Card(string name, string type, int buffAmount, int hp, int damage, int cost);

    Card(string name, string type, int hp, int damage, int cost);

    Card() = default;

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

    void setPosition(int x, int y);

    void setPosition(Rectangle rec);

    void init();

    bool operator==(const Card &other) const = default;

private:
    string m_name{};
    string m_type{};
    int m_buffAmount{};
    int m_hp{};
    int m_damage{};
    int m_cost{};
    int m_x{};
    int m_y{};
    int m_width{};
    int m_height{};
    bool m_hasAttacked{};
};
