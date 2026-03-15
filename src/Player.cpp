#include "Player.hpp"
#include "FloorMap.hpp" // Required to get target entities for interaction
#include "Util/Image.hpp"
#include "Util/Logger.hpp"

Player::Player()
    : Entity(0, MAGIC_TOWER_RESOURCE_DIR "/bmp/Player/player_backward.png",
             false) { // Player defaults to not reacting (reaction() disabled)
  // Player layer is -3 based on Constructure.md
  SetZIndex(-3.0f);
  SetVisible(true);

  // Initial grid position
  m_GridX = 5;
  m_GridY = 9;

  m_YellowKeys = 2;
  m_BlueKeys = 1;
  m_RedKeys = 1;  
}

void Player::Move(int dx, int dy, std::shared_ptr<FloorMap> roadmap,
                  std::shared_ptr<FloorMap> thingsmap) {
  int nextX = m_GridX + dx;
  int nextY = m_GridY + dy;

  // Basic bounds checking for 11x11 grid
  if (nextX < 0 || nextX >= 11 || nextY < 0 || nextY >= 11) {
    return;
  }

  // Check collision with roadmap
  if (roadmap && !roadmap->IsPassable(nextX, nextY)) {
    return;
  }

  // Check collision with thingsMap (interactions)
  if (thingsmap) {
    auto target = thingsmap->GetObject(nextX, nextY);
    auto entity = std::dynamic_pointer_cast<Entity>(target);
    if (entity && entity->GetVisible()) {
      // Trigger reaction if possible - passing player pointer
      if (entity->CanReact()) {
        entity->reaction(shared_from_this());
      }

      // Block movement if the entity is still visible and not passable
      if (entity->GetVisible() && !entity->IsPassable()) {
        return;
      }
    }
  }

  m_GridX = nextX;
  m_GridY = nextY;

  SyncPosition(roadmap);
}

void Player::reaction(std::shared_ptr<Player> player) {
  LOG_INFO("Player triggered reaction()! Possible mirror stage.");
}

void Player::AddKey(int id) {
  switch (id) {
  case 201:
    m_YellowKeys++;
    break;
  case 202:
    m_BlueKeys++;
    break;
  case 203:
    m_RedKeys++;
    break;
  default:
    LOG_WARN("Player::AddKey: Unknown key ID {}", id);
    break;
  }
}

bool Player::UseKey(int doorId) {
  switch (doorId) {
  case 302: // Yellow Door
    if (m_YellowKeys > 0) {
      m_YellowKeys--;
      return true;
    }
    break;
  case 303: // Blue Door
    if (m_BlueKeys > 0) {
      m_BlueKeys--;
      return true;
    }
    break;
  case 304: // Red Door
    if (m_RedKeys > 0) {
      m_RedKeys--;
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

  auto obj = roadmap->GetObject(m_GridX, m_GridY);
  if (obj) {
    // Borrow transform (translation and scale)
    m_Transform = obj->m_Transform;
  } else {
    LOG_ERROR("Player::SyncPosition: Could not find object at {}, {}", m_GridX,
              m_GridY);
  }
}
