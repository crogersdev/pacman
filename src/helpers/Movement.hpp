#include <array>

#include <raylib.h>

const std::array<Vector2, 4> directionLut = {{
    {  0., -1. },  // UP
    {  0.,  1. },  // DOWN
    { -1.,  0. },  // LEFT
    {  1.,  0. }   // RIGHT
}};
