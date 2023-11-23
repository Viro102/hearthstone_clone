#ifndef RAYLIB_TEST_CARD_H
#define RAYLIB_TEST_CARD_H

#include "Common.h"

class Card {
public:
    Card(string name, string type, int buffAmount, int hp, int damage, int cost);

    Card(string name, string type, int hp, int damage, int cost);

    string getName() const;

    string getType() const;

    int getHp() const;

    int getDamage() const;

    int getCost() const;

    int getBuffAmount() const;

    int getX() const;

    int getY() const;

    int getHeight() const;

    int getWidth() const;

    const Rectangle &getShape() const;

    bool getHasAttacked() const;

    void setHasAttacked(bool hasAttacked);

    void setHp(int hp);

    void setDamage(int damage);

    void setX(int x);

    void setY(int y);

    void setPosition(int x, int y);

    void init();

    bool operator==(const Card &other) const;


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

#endif //RAYLIB_TEST_CARD_H
