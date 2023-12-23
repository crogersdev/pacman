#pragma once

#include <SFML/Graphics.hpp>

#include "../entities/Labyrinth.hpp"

bool wallCollides(sf::Vector2f, sf::Vector2f, const Labyrinth &);
bool entityCollides(sf::RectangleShape, sf::RectangleShape);