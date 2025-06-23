#include <map>

#include <raylib.h>

enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

const std::map<Direction, Vector2> directionLut = {
   { Direction::UP,    {  0.f, -1.f } },
   { Direction::DOWN,  {  0.f,  1.f } },
   { Direction::LEFT,  { -1.f,  0.f } },
   { Direction::RIGHT, {  1.f,  0.f } }
};
