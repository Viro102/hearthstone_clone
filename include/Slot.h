#pragma once

#include "Common.h"

class Slot {
public:
    Slot();

    Slot(int x, int y);

    bool isGlow() const;

    void setGlow(bool glow);

    bool isFree() const;

    void setFree(bool free);

    int getX() const;

    int getY() const;

    int getWidth() const;

    int getHeight() const;

    const Rectangle getShape() const;

private:
    int m_x;
    int m_y;
    int m_width{150};
    int m_height{200};
    bool m_free;
    bool m_glow;
};
