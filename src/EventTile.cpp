#include "EventTile.hpp"
#include "Player.hpp"
#include "Util/Logger.hpp"


EventTile::EventTile() {
  is_passable = true; // Events are normally passable to trigger them
}

void EventTile::onTrigger(Player *player) {
  if (!player)
    return;

  // Default trigger logic.
  LOG_TRACE("Event tile of type " + event_type + " triggered.");
}
