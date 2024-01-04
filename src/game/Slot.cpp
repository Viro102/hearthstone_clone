#include "../../include/Slot.h"

Slot::Slot(int x, int y) : m_x(x), m_y(y) {}

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

Rectangle Slot::getShape() const {
    return Rectangle(m_x, m_y, m_width, m_height);
}
