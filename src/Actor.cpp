#include "Actor.hpp"

Actor::Actor() : x(0), y(0) {}

bool Actor::canMove(int delta_x, int delta_y) {
  if (!is_movable)
    return false;

  // TODO: Add collision checking with Tile map
  return true;
}
