#ifndef EVENTTILE_HPP
#define EVENTTILE_HPP

#include "Tile.hpp"
#include <functional>
#include <string>

class Entity;

// For interactive environments like stairs, teleports or invisible walls
class EventTile : public Tile {
public:
  EventTile();
  ~EventTile() = default;

  // Trigger action when Player steps on or interacts with it
  virtual void onTrigger(Entity *player);

  std::string getEventType() const { return event_type; }
  void setEventType(const std::string &type) { event_type = type; }

private:
  std::string event_type = "NONE"; // e.g., "STAIR_UP", "STAIR_DOWN", "DIALOGUE"
};

#endif // EVENTTILE_HPP
