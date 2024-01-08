#pragma once

#include <raylib.h>
#include <Common.h>
#include <functional>
#include <utility>

class Button {
public:
    Button();

    Button(int x, int y, Texture2D texture);

    Button(int x, int y);

    explicit Button(const Rectangle &bounds, string text = "");

    void draw() const;

    void setFree(bool isFree);

    void setColor(Color color);

    [[nodiscard]] bool isClicked() const;

    [[nodiscard]] bool isFree() const;

    [[nodiscard]] Rectangle getHitbox() const;

private:
    Rectangle m_hitbox;
    string m_text{};
    Texture2D m_texture{};
    int m_fontSize{20};
    int m_centerX{};
    int m_centerY{};
    Color m_regularColor{GRAY};
    Color m_hoverColor{DARKGRAY};
    Color m_textColor{BLACK};
    bool m_visible{true};
    bool m_isFree{true};
};
