#pragma once

#include <cmath>
#include <SFML/Graphics.hpp>

void wrapCoordinate(float &, float min, float);
std::pair<int, int> tileCoordsAtPosition(sf::Vector2f);