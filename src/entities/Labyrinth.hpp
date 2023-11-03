#pragma once

#include <SFML/Graphics.hpp>

#include <array>

const int LABYRINTH_ROWS = 15;
const int LABYRINTH_COLS = 10;

class Labyrinth
{
public:
    int m_labyrinthRows;
    int m_labyrinthCols;

    std::array<std::array<int, LABYRINTH_COLS>, LABYRINTH_ROWS> m_labyrinth =
        {{
            {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
            {{1, 0, 0, 0, 0, 0, 0, 0, 1, 1}},
            {{1, 0, 0, 0, 0, 0, 0, 1, 0, 1}},
            {{1, 0, 0, 0, 0, 0, 1, 0, 0, 1}},
            {{1, 0, 0, 0, 0, 1, 0, 0, 0, 1}},
            {{1, 0, 0, 0, 1, 0, 0, 0, 0, 1}},
            {{1, 0, 0, 0, 0, 0, 0, 0, 0, 1}},
            {{1, 0, 0, 0, 0, 0, 0, 0, 0, 1}},
            {{1, 0, 0, 0, 0, 0, 0, 0, 0, 1}},
            {{1, 0, 0, 0, 0, 0, 0, 0, 0, 1}},
            {{1, 0, 0, 0, 0, 0, 0, 0, 0, 1}},
            {{1, 0, 0, 0, 0, 0, 0, 0, 0, 1}},
            {{1, 0, 0, 0, 0, 0, 0, 0, 0, 1}},
            {{1, 0, 0, 0, 0, 0, 0, 0, 0, 1}},
            {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}
        }};

    std::array<int, LABYRINTH_COLS * LABYRINTH_ROWS> m_levelOne;

    Labyrinth();
};