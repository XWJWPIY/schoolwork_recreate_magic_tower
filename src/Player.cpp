#include "Player.hpp"
#include "FloorMap.hpp" // Required to get target entities for interaction
#include "Util/Image.hpp"
#include "Util/Logger.hpp"

Player::Player()
    : Entity(0, MAGIC_TOWER_RESOURCE_DIR "/bmp/Player/player_backward.png",
             false) { // Player defaults to not reacting (Reaction() disabled)
  // Player layer is -3 based on Constructure.md
  SetZIndex(-3.0f);
  SetVisible(true);

  // Initial grid position
  m_grid_x = 5;
  m_grid_y = 9;

  // Initial Stats from reference UI
  m_hp = 1000;
  m_attack = 10;
  m_defense = 10;
  m_agility = 2;
  m_exp = 0;

  m_yellow_keys = 1;
  m_blue_keys = 1;
  m_red_keys = 1;  
  m_coins = 0;
}

void Player::Move(int dx, int dy, std::shared_ptr<FloorMap> roadmap,
                  std::shared_ptr<FloorMap> thingsmap) {
  int next_x = m_grid_x + dx;
  int next_y = m_grid_y + dy;

  // Basic bounds checking for 11x11 grid
  if (next_x < 0 || next_x >= 11 || next_y < 0 || next_y >= 11) {
    return;
  }

  // Check collision with roadmap
  if (roadmap && !roadmap->IsPassable(next_x, next_y)) {
    return;
  }

  // Check collision with thingsMap (interactions)
  if (thingsmap) {
    auto target = thingsmap->GetObject(next_x, next_y);
    auto entity = std::dynamic_pointer_cast<Entity>(target);
    if (entity && entity->GetVisible()) {
      // Trigger reaction if possible - passing player pointer
      if (entity->CanReact()) {
        entity->Reaction(shared_from_this());
      }

      // Block movement if the entity is still visible and not passable
      if (entity->GetVisible() && !entity->IsPassable()) {
        return;
      }
    }
  }

  m_grid_x = next_x;
  m_grid_y = next_y;

  SyncPosition(roadmap);
}

void Player::Reaction(std::shared_ptr<Player> player) {
  LOG_INFO("Player triggered Reaction()! Possible mirror stage.");
}

void Player::AddKey(int id) {
  switch (id) {
  case 201:
    m_yellow_keys++;
    break;
  case 202:
    m_blue_keys++;
    break;
  case 203:
    m_red_keys++;
    break;
  default:
    LOG_WARN("Player::AddKey: Unknown key ID {}", id);
    break;
  }
}

bool Player::UseKey(int doorId) {
  switch (doorId) {
  case 302: // Yellow Door
    if (m_yellow_keys > 0) {
      m_yellow_keys--;
      return true;
    }
    break;
  case 303: // Blue Door
    if (m_blue_keys > 0) {
      m_blue_keys--;
      return true;
    }
    break;
  case 304: // Red Door
    if (m_red_keys > 0) {
      m_red_keys--;
      return true;
    }
    break;
  default:
    LOG_WARN("Player::UseKey: Unknown door ID {}", doorId);
    break;
  }
  return false;
}

void Player::SyncPosition(std::shared_ptr<FloorMap> roadmap) {
  if (!roadmap)
    return;

  auto obj = roadmap->GetObject(m_grid_x, m_grid_y);
  if (obj) {
    // Borrow transform (translation and scale)
    m_Transform = obj->m_Transform;
  } else {
    LOG_ERROR("Player::SyncPosition: Could not find object at {}, {}", m_grid_x,
              m_grid_y);
  }
}
