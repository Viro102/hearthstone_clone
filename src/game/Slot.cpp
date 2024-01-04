#include "../../include/Slot.h"

Slot::Slot() : m_x(0), m_y(0), m_free(true), m_glow(false) {}

Slot::Slot(int x, int y) : m_x(x), m_y(y), m_free(true), m_glow(false) {}

bool Slot::isGlow() const {
    return m_glow;
}

void Slot::setGlow(bool glow) {
    m_glow = glow;
}

bool Slot::isFree() const {
    return m_free;
}

void Slot::setFree(bool free) {
    m_free = free;
}

int Slot::getX() const {
    return m_x;
}

int Slot::getY() const {
    return m_y;
}

int Slot::getWidth() const {
    return m_width;
}

int Slot::getHeight() const {
    return m_height;
}

const Rectangle Slot::getShape() const {
    return Rectangle(m_x, m_y, m_width, m_height);
}
