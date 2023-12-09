#pragma once

#include <cmath>
#include <SFML/Graphics.hpp>
#include "../game-manager/GameManager.hpp"

std::pair<int, int> positionToCoords(sf::Vector2f);
sf::Vector2f tileCoordsAtPosition(sf::Vector2f);
void wrapCoordinate(float &, float min, float);