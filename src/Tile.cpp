#include "Tile.hpp"

Tile::Tile() : x(0), y(0) {}

bool Tile::isPassable() const { return is_passable; }

void Tile::setPassable(bool passable) { is_passable = passable; }
