#ifndef STATICTILE_HPP
#define STATICTILE_HPP

#include "Tile.hpp"

// For basic walls, floors or untraversable obstacles
class StaticTile : public Tile {
public:
  StaticTile(bool passable = false);
  ~StaticTile() override = default;
};

#endif // STATICTILE_HPP
