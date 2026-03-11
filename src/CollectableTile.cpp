#include "CollectableTile.hpp"
#include "Entity.hpp"
#include "Util/Logger.hpp"

CollectableTile::CollectableTile() {
  is_passable = true; // Collectables must be stepped on to be collected
}

bool CollectableTile::onCollect(Entity *player) {
  if (!player)
    return false;

  // Collect logic placeholder
  LOG_TRACE("Collected item of type " + object_type);

  return true; // Return true means it should be deleted from map
}
