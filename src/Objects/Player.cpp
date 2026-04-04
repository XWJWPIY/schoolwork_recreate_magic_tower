#include "Objects/Player.hpp"
#include "Core/AppUtil.hpp"
#include "Core/FloorMap.hpp" // Required to get target entities for interaction
#include "Util/Image.hpp"
#include "Util/Logger.hpp"

Player::Player()
    : Actor(0, false) {
  // Player layer is -3 based on Constructure.md
  SetZIndex(-3.0f);

  // Initial grid position
  m_grid_x = 5;
  m_grid_y = 10;

  // Initial Stats from reference UI
  SetAttr(AppUtil::Effect::HP, 1000);
  SetAttr(AppUtil::Effect::ATTACK, 10);
  SetAttr(AppUtil::Effect::DEFENSE, 10);
  SetAttr(AppUtil::Effect::AGILITY, 2);
  SetAttr(AppUtil::Effect::EXP, 0);
  SetAttr(AppUtil::Effect::LEVEL, 1);

  SetAttr(AppUtil::Effect::KEY_YELLOW, 1);
  SetAttr(AppUtil::Effect::KEY_BLUE, 1);
  SetAttr(AppUtil::Effect::KEY_RED, 1);
  SetAttr(AppUtil::Effect::COIN, 0);
  SetAttr(AppUtil::Effect::ENEMY_BOOK, 0);
  SetAttr(AppUtil::Effect::HIGHEST_FLOOR, 0);

  // Pre-load 4-directional animations (4 frames each)
  for (int i = 1; i <= 4; i++) {
    std::string basePath = "bmp/Player/player_" + std::to_string(i);
    std::vector<std::string> paths;
    for (int f = 1; f <= 4; f++) {
      paths.push_back(AppUtil::GetPhaseImagePath(basePath, f));
    }
    // interval 50ms (FRAME_INTERVAL was 0.05f), looping = false
    m_animations[i - 1] = std::make_shared<Util::Animation>(paths, false, 50, false);
  }

  // Set initial state
  SetDirection(PlayerDirection::DOWN);
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
    SetDirection(m_direction);
    return;
  }

  // Check collision with roadmap
  if (roadmap && !roadmap->IsPassable(next_x, next_y)) {
    m_is_animating = false;
    SetDirection(m_direction); // Still update direction to face obstacle
    return;
  }
  // Check collision with thingsMap (interactions)
  if (thingsmap) {
    auto target = thingsmap->GetObject(next_x, next_y);
    if (target) {
      auto entity = std::dynamic_pointer_cast<Entity>(target);
      if (entity && entity->GetVisible()) {
        if (entity->CanReact()) {
          if (entity->CheckCondition(std::static_pointer_cast<Player>(shared_from_this()))) {
            entity->Reaction(std::static_pointer_cast<Player>(shared_from_this()));
            
            if (entity->InterruptsMovementSync()) {
                LOG_INFO("Player Move: Movement sync interrupted by entity.");
                return;
            }
          } else {
            m_is_animating = false;
            SetDirection(m_direction);
            return;
          }
        }

        if (entity->GetVisible() && !entity->IsPassable()) {
          m_is_animating = false;
          SetDirection(m_direction);
          return;
        }

        // If it was a passable entity that triggered a floor change (like a stair),
        // we might have already been moved or reset. Check if we still want to animate.
        if (entity->ShouldSkipWalkAnimation()) {
          m_grid_x = next_x;
          m_grid_y = next_y;
          m_is_animating = false;
          SyncPosition(roadmap);
          SetDirection(m_direction);
          return;
        }
      }
    }
  }

  // Success move
  m_grid_x = next_x;
  m_grid_y = next_y;

  m_is_animating = true;
  SetDirection(m_direction);
  m_animations[static_cast<int>(m_direction) - 1]->Play();

  SyncPosition(roadmap);
}

void Player::Reaction(std::shared_ptr<Player> player) {
  LOG_INFO("Player triggered Reaction()! Possible mirror stage.");
}

void Player::ResetStateAfterFloorChange() {
  m_direction = PlayerDirection::DOWN;
  m_is_animating = false;
  if (m_is_super_mode && m_giraffe_image) {
    SetDrawable(m_giraffe_image);
  } else {
    // Use the specific transparent sprite for floor changes (usually on stairs)
    m_Drawable = std::make_shared<Util::Image>(AppUtil::GetStaticResourcePath("bmp/Player/player_1.png"));
  }
}

void Player::SyncPosition(std::shared_ptr<FloorMap> roadmap) {
  if (!roadmap)
    return;

  m_Transform.translation = roadmap->GetGridAbsolutePosition(m_grid_x, m_grid_y);

  if (m_is_super_mode) {
    // Giraffe image is 512x512, scale it down to fit a single grid cell (~48px)
    float gridScale = roadmap->GetScale().x;
    float giraffeRatio = AppUtil::Skin::SUPER_MODE_RATIO;
    m_Transform.scale = {gridScale * giraffeRatio, gridScale * giraffeRatio};
  } else {
    m_Transform.scale = roadmap->GetScale();
  }
}

void Player::OnAttributeChanged(AppUtil::Effect type) {
    if (type == AppUtil::Effect::HP && GetAttr(AppUtil::Effect::HP) <= 0) {
        LOG_INFO("Player has died!");
        // TODO: Trigger Game Over
    }
}


void Player::ObjectUpdate() {
  if (m_is_super_mode) {
    // In super mode, giraffe is always static — no walk animation
    m_is_animating = false;
    return;
  }
  if (m_is_animating) {
    auto currentAnim = m_animations[static_cast<int>(m_direction) - 1];
    if (currentAnim->GetState() == Util::Animation::State::ENDED) {
      m_is_animating = false;
      SetDirection(m_direction); // Return to stationary transparent sprite
    }
  }
}

// Special wrapper to switch drawable along with direction
void Player::SetDirection(PlayerDirection dir) {
  m_direction = dir;
  if (m_is_super_mode) {
    // Super mode: always show giraffe, don't change drawable for direction
    return;
  }
  if (m_is_animating) {
    SetDrawable(m_animations[static_cast<int>(m_direction) - 1]);
  } else {
    // If not animating (failed move, stood on stair, etc.), use transparent PNG
    std::string path = "bmp/Player/player_" + std::to_string(static_cast<int>(dir)) + ".png";
    m_Drawable = std::make_shared<Util::Image>(AppUtil::GetStaticResourcePath(path));
  }
}

int Player::GetAttr(AppUtil::Effect type) const {
    if (m_is_super_mode && type != AppUtil::Effect::HIGHEST_FLOOR) {
        auto it = m_super_attributes.find(type);
        if (it != m_super_attributes.end()) return it->second;
    }
    return Actor::GetAttr(type);
}

void Player::SetAttr(AppUtil::Effect type, int value) {
    if (m_is_super_mode && type != AppUtil::Effect::HIGHEST_FLOOR) {
        m_super_attributes[type] = value;
        OnAttributeChanged(type);
    } else {
        Actor::SetAttr(type, value);
    }
}

void Player::ApplyEffect(AppUtil::Effect type, int delta) {
    if (m_is_super_mode && type != AppUtil::Effect::HIGHEST_FLOOR) {
        m_super_attributes[type] += delta;
        OnAttributeChanged(type);
    } else {
        Actor::ApplyEffect(type, delta);
    }
}

void Player::ToggleSuperMode() {
    m_is_super_mode = !m_is_super_mode;

    if (m_is_super_mode && m_super_attributes.empty()) {
        LOG_INFO("Super Mode: Initializing default super stats...");
        m_super_attributes[AppUtil::Effect::HP] = 999999;
        m_super_attributes[AppUtil::Effect::ATTACK] = 999;
        m_super_attributes[AppUtil::Effect::DEFENSE] = 999;
        m_super_attributes[AppUtil::Effect::AGILITY] = 50;
        m_super_attributes[AppUtil::Effect::ENEMY_BOOK] = 1;
        m_super_attributes[AppUtil::Effect::FLY] = 1;

        // Copy current consumable resources to the super bucket so they are available
        m_super_attributes[AppUtil::Effect::KEY_YELLOW] = Actor::GetAttr(AppUtil::Effect::KEY_YELLOW);
        m_super_attributes[AppUtil::Effect::KEY_BLUE] = Actor::GetAttr(AppUtil::Effect::KEY_BLUE);
        m_super_attributes[AppUtil::Effect::KEY_RED] = Actor::GetAttr(AppUtil::Effect::KEY_RED);
        m_super_attributes[AppUtil::Effect::COIN] = 1000;
        m_super_attributes[AppUtil::Effect::EXP] = 1000;
    }

    // Switch skin
    if (m_is_super_mode) {
        if (!m_giraffe_image) {
            m_giraffe_image = std::make_shared<Util::Image>(
                AppUtil::GetStaticResourcePath(AppUtil::Skin::SUPER_MODE_PATH));
        }
        SetDrawable(m_giraffe_image);
    } else {
        // Restore normal player sprite
        SetDirection(m_direction);
    }

    LOG_INFO("Super Mode: {}", m_is_super_mode ? "ACTIVATED (Giraffe)" : "DEACTIVATED (Normal)");
}
