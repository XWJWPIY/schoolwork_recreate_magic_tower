#include "Player.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"

Player::Player() : Entity(0) {
  // Set Player specific image
  auto temp = std::dynamic_pointer_cast<Util::Image>(m_Drawable);
  if (temp) {
    temp->SetImage(MAGIC_TOWER_RESOURCE_DIR "/bmp/Player/player_backward.png");
  }

  // Player layer is -3 based on Constructure.md
  SetZIndex(-3.0f);
  SetVisible(true);

  // Initial grid position
  m_GridX = 5;
  m_GridY = 9;
}

void Player::Move(int dx, int dy, std::shared_ptr<FloorMap> roadmap) {
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

  // TODO: Check collision with thingsMap (interactions)

  m_GridX = nextX;
  m_GridY = nextY;

  SyncPosition(roadmap);
}

void Player::SyncPosition(std::shared_ptr<FloorMap> roadmap) {
  if (!roadmap)
    return;

  auto block = roadmap->GetBlock(m_GridX, m_GridY);
  if (block) {
    // Borrow transform (translation and scale)
    m_Transform = block->m_Transform;
  } else {
    LOG_ERROR("Player::SyncPosition: Could not find block at {}, {}", m_GridX,
              m_GridY);
  }
}
