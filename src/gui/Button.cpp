#include <Button.h>

Button::Button() : m_hitbox() {}

Button::Button(int x, int y, Texture2D texture)
        : m_hitbox(Rectangle(x, y, texture.width, texture.height)),
          m_texture(texture),
          m_regularColor(BLANK),
          m_hoverColor(YELLOW),
          m_visible(false),
          m_isFree(false) {}

Button::Button(int x, int y) : m_hitbox(Rectangle(x, y, 150, 200)),
                               m_regularColor(BLANK), m_hoverColor(YELLOW), m_visible(false) {}


Button::Button(const Rectangle &bounds, string text) : m_hitbox(bounds), m_text(std::move(text)) {
    m_centerX = static_cast<int>(m_hitbox.x + m_hitbox.width / 2);
    m_centerY = static_cast<int>(m_hitbox.y + m_hitbox.height / 2);
}

void Button::draw() const {
    if (m_visible) {
        DrawRectangleRec(m_hitbox,
                         CheckCollisionPointRec(GetMousePosition(), m_hitbox) ? m_hoverColor : m_regularColor);
        int width = MeasureText(m_text.c_str(), m_fontSize);
        DrawText(m_text.c_str(), m_centerX - width / 2, m_centerY - m_fontSize / 2, m_fontSize, m_textColor);
    } else {
        DrawTexture(m_texture, m_hitbox.x, m_hitbox.y, WHITE);
    }

    if (!isFree()) {
        DrawRectangleLinesEx(m_hitbox, 2, CheckCollisionPointRec(GetMousePosition(), m_hitbox)
                                          ? m_hoverColor : m_regularColor);
    }
}

void Button::update() const {
    if (CheckCollisionPointRec(GetMousePosition(), m_hitbox)
        && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && onClick) {
        onClick();
    }
}

void Button::setOnClick(std::function<void()> func) {
    onClick = std::move(func);
}

bool Button::isFree() const {
    return m_isFree;
}

void Button::setFree(bool isFree) {
    m_isFree = isFree;
}

Rectangle Button::getHitbox() const {
    return m_hitbox;
}
