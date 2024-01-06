#pragma once

#include <Common.h>
#include <raylib.h>

class Slot {
public:
    Slot() = default;

    Slot(int x, int y);

    [[nodiscard]] bool isGlow() const;

    void setGlow(bool glow);

    [[nodiscard]] bool isFree() const;

    void setFree(bool free);

    [[nodiscard]] Rectangle getShape() const;

private:
    int m_x{0};
    int m_y{0};
    int m_width{150};
    int m_height{200};
    bool m_free{true};
    bool m_glow{false};
};
