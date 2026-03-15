#include "Enemy.hpp"
#include "AppUtil.hpp"
#include "Player.hpp"
#include "Util/Logger.hpp"

Enemy::Enemy(int id)
    : Entity(id, "", true) {}

void Enemy::reaction(std::shared_ptr<Player> player) {
  LOG_INFO("Entering battle! Enemy ID: {} ({})", m_ObjectId,
           AppUtil::GetIdString(m_ObjectId));
  // TODO: Battle logic and damage calculation

  if (m_ReplacementComp) {
    m_ReplacementComp->ReplaceWith(m_GridX, m_GridY, 0); // Replace with empty floor
  } else {
    SetVisible(false);
  }
}
