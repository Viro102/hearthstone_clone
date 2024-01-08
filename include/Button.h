#pragma once

#include <raylib.h>
#include <Common.h>
#include <functional>
#include <utility>

class Button {
public:
    Button(const Rectangle &bounds, string text);

    void draw() const;

    void update() const;

    void setOnClick(std::function<void()> func);

private:
    Rectangle m_hitbox;
    string m_text;
    int m_fontSize{20};
    int m_centerX{};
    int m_centerY{};
    Color m_regularColor{GRAY};
    Color m_hoverColor{DARKGRAY};
    Color m_textColor{BLACK};
    std::function<void()> onClick;
};
