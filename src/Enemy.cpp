#include "Enemy.hpp"
#include "AppUtil.hpp"
#include "Player.hpp"
#include "Util/Logger.hpp"

Enemy::Enemy(int id)
    : Actor(id, "", true) {}

void Enemy::Reaction(std::shared_ptr<Player> player) {
  LOG_INFO("Entering battle! Enemy ID: {} ({})", m_object_id,
           AppUtil::GetIdString(m_object_id));
  // TODO: Battle logic and damage calculation

  if (m_replacement_comp) {
    m_replacement_comp->ReplaceWith(m_grid_x, m_grid_y, 0); // Replace with empty floor
  } else {
    SetVisible(false);
  }
}
