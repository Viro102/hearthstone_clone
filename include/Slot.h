#pragma once

#include <Common.h>

class Slot {
public:
    Slot() = default;

    Slot(int x, int y);

    bool isGlow() const;

    void setGlow(bool glow);

    bool isFree() const;

    void setFree(bool free);

    Rectangle getShape() const;

private:
    int m_x{0};
    int m_y{0};
    int m_width{150};
    int m_height{200};
    bool m_free{true};
    bool m_glow{false};
};
