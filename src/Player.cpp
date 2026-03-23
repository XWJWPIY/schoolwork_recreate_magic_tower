#include "Player.hpp"
#include "FloorMap.hpp" // Required to get target entities for interaction
#include "Util/Image.hpp"
#include "Util/Logger.hpp"

Player::Player()
    : Actor(0, MAGIC_TOWER_RESOURCE_DIR "/bmp/Player/player_11.BMP",
             false) { // Start facing Down, Frame 1
  // Player layer is -3 based on Constructure.md
  SetZIndex(-3.0f);

  // Initial grid position
  m_grid_x = 5;
  m_grid_y = 10;

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

  UpdateSprite();
}

void Player::Move(int dx, int dy, std::shared_ptr<FloorMap> roadmap,
                  std::shared_ptr<FloorMap> thingsmap) {
  // Update direction based on input
  if (dx > 0) m_direction = PlayerDirection::RIGHT;
  else if (dx < 0) m_direction = PlayerDirection::LEFT;
  else if (dy > 0) m_direction = PlayerDirection::DOWN;
  else if (dy < 0) m_direction = PlayerDirection::UP;

  int next_x = m_grid_x + dx;
  int next_y = m_grid_y + dy;

  // Animation trigger (always start animation, but if blocked it will stay on frame 1)
  // Actually as per requirement: Failure -> Face direction but stay on frame 1.
  // Success -> 1-4 cycle.

  // Basic bounds checking for 11x11 grid
  if (next_x < 0 || next_x >= 11 || next_y < 0 || next_y >= 11) {
    m_is_animating = false;
    m_current_frame = 1;
    UpdateSprite();
    return;
  }

  // Check collision with roadmap
  if (roadmap && !roadmap->IsPassable(next_x, next_y)) {
    m_is_animating = false;
    m_current_frame = 1;
    UpdateSprite();
    return;
  }

  // Check collision with thingsMap (interactions)
  if (thingsmap) {
    auto target = thingsmap->GetObject(next_x, next_y);
    if (target) {
      auto it = AppUtil::GlobalObjectRegistry.find(target->GetObjectId());
      if (it != AppUtil::GlobalObjectRegistry.end() && it->second.stair_props) {
        // It's a stair! Move but don't animate according to requirement.
        m_grid_x = next_x;
        m_grid_y = next_y;
        m_is_animating = false;
        m_current_frame = 1;
        UpdateSprite();
        SyncPosition(roadmap);
        
        auto stairEntity = std::dynamic_pointer_cast<Entity>(target);
        if (stairEntity) stairEntity->Reaction(std::static_pointer_cast<Player>(shared_from_this()));
        return;
      }

      auto entity = std::dynamic_pointer_cast<Entity>(target);
      if (entity && entity->GetVisible()) {
        if (entity->CanReact()) {
          entity->Reaction(std::static_pointer_cast<Player>(shared_from_this()));
        }

        if (entity->GetVisible() && !entity->IsPassable()) {
          m_is_animating = false;
          m_current_frame = 1;
          UpdateSprite();
          return;
        }
      }
    }
  }

  // Success move
  m_grid_x = next_x;
  m_grid_y = next_y;

  m_is_animating = true;
  m_current_frame = 1;
  m_animation_timer = 0.0f;
  UpdateSprite();

  SyncPosition(roadmap);
}

void Player::Reaction(std::shared_ptr<Player> player) {
  LOG_INFO("Player triggered Reaction()! Possible mirror stage.");
}

bool Player::UseKey(AppUtil::Effect type, int count) {
  switch (type) {
  case AppUtil::Effect::KEY_YELLOW:
    if (m_yellow_keys >= count) {
      m_yellow_keys -= count;
      return true;
    }
    break;
  case AppUtil::Effect::KEY_BLUE:
    if (m_blue_keys >= count) {
      m_blue_keys -= count;
      return true;
    }
    break;
  case AppUtil::Effect::KEY_RED:
    if (m_red_keys >= count) {
      m_red_keys -= count;
      return true;
    }
    break;
  default:
    LOG_WARN("Player::UseKey: Unsupported effect type for key consumption");
    break;
  }
  return false;
}

void Player::ResetStateAfterFloorChange() {
  m_direction = PlayerDirection::DOWN;
  m_is_animating = false;
  m_current_frame = 1;
  UpdateSprite();
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

void Player::ApplyEffect(AppUtil::Effect type, int value) {
  switch (type) {
  case AppUtil::Effect::HP:
    m_hp += value;
    break;
  case AppUtil::Effect::ATTACK:
    m_attack += value;
    break;
  case AppUtil::Effect::DEFENSE:
    m_defense += value;
    break;
  case AppUtil::Effect::AGILITY:
    m_agility += value;
    break;
  case AppUtil::Effect::KEY_YELLOW:
    m_yellow_keys += value;
    break;
  case AppUtil::Effect::KEY_BLUE:
    m_blue_keys += value;
    break;
  case AppUtil::Effect::KEY_RED:
    m_red_keys += value;
    break;
  case AppUtil::Effect::COIN:
    m_coins += value;
    break;
  case AppUtil::Effect::LEVEL:
    m_level += value;
    LOG_INFO("Player level increased by {}", value);
    break;
  case AppUtil::Effect::EXP:
    m_exp += value;
    LOG_INFO("Player EXP increased by {}", value);
    break;
  case AppUtil::Effect::WEAK:
    // TODO: Implement weak status logic
    LOG_INFO("Player current weak status: {}", value);
    break;
  case AppUtil::Effect::POISON:
    // TODO: Implement poison status logic
    LOG_INFO("Player current poison status: {}", value);
    break;
  case AppUtil::Effect::FLY:
    m_has_fly = true;
    LOG_INFO("Player acquired FLY effect");
    break;
  case AppUtil::Effect::NONE:
  default:
    break;
  }
}

void Player::ObjectUpdate() {
  if (m_is_animating) {
    m_animation_timer += static_cast<float>(Util::Time::GetDeltaTime());
    if (m_animation_timer >= FRAME_INTERVAL) {
      m_animation_timer = 0.0f;
      m_current_frame++;
      if (m_current_frame > 4) {
        m_current_frame = 1;
        m_is_animating = false; // Stop animation after one full loop
      }
      UpdateSprite();
    }
  }
}

void Player::UpdateSprite() {
  std::string path;
  if (!m_is_animating) {
    // Static standing frame
    path = MAGIC_TOWER_RESOURCE_DIR "/bmp/Player/player_" +
           std::to_string(static_cast<int>(m_direction)) + ".png";
  } else {
    // Animated moving frames
    path = MAGIC_TOWER_RESOURCE_DIR "/bmp/Player/player_" +
           std::to_string(static_cast<int>(m_direction)) +
           std::to_string(m_current_frame) + ".BMP";
  }
  
  // Dynamically change the drawable (Image)
  m_Drawable = std::make_shared<Util::Image>(path);
}
