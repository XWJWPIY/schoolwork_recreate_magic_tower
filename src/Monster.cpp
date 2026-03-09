#include "Monster.hpp"
#include "Player.hpp"
#include "Util/Logger.hpp"

Monster::Monster() {
  is_movable = false; // Most monsters are stationary
}

bool Monster::onBattle(Player *player) {
  if (!player)
    return false;

  // Simple fixed-value turn-based battle formula placeholder
  // (Player attack - Monster defense) vs (Monster attack - Player defense)
  // TODO: Implement actual battle logic
  LOG_TRACE("Battle initiated between player and monster.");

  return true; // Return true if player wins
}
