#include <Button.h>

Button::Button(const Rectangle &bounds, string text) : m_hitbox(bounds), m_text(std::move(text)) {
    m_centerX = static_cast<int>(m_hitbox.x + m_hitbox.width / 2);
    m_centerY = static_cast<int>(m_hitbox.y + m_hitbox.height / 2);
}

void Button::draw() const {
    DrawRectangleRec(m_hitbox, CheckCollisionPointRec(GetMousePosition(), m_hitbox) ? m_hoverColor : m_regularColor);
    int width = MeasureText(m_text.c_str(), m_fontSize);
    DrawText(m_text.c_str(), m_centerX - width / 2, m_centerY - m_fontSize / 2, m_fontSize, m_textColor);
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
