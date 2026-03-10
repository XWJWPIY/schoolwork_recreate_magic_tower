#ifndef COLLECTABLETILE_HPP
#define COLLECTABLETILE_HPP

#include "Tile.hpp"
#include <string>

class Actor;

// For items that the player can pick up, e.g., Keys, Potions, Gems
class CollectableTile : public Tile {
public:
  CollectableTile();
  ~CollectableTile() = default;

  // Return true if collection was successful, false if inventory full or unmet
  // condition
  virtual bool onCollect(Actor *player);

  std::string getItemType() const { return object_type; }
  void setItemType(const std::string &type) { object_type = type; }

private:
  std::string object_type = "NONE"; // e.g., "RED_KEY", "RED_GEM", "HP_POTION"
};

#endif // COLLECTABLETILE_HPP
