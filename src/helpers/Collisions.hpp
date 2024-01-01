#pragma once

#include <SFML/Graphics.hpp>

#include <vector>

#include "../entities/Labyrinth.hpp"
#include "../helpers/TileCoordConversion.hpp"

std::vector<Direction> availableTurns(sf::Vector2f, sf::Vector2f, const Labyrinth &);
bool entityCollides(sf::RectangleShape, sf::RectangleShape);
bool wallCollides(sf::Vector2f, sf::Vector2f, const Labyrinth &);