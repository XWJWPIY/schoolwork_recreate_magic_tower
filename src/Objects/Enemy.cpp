#include "Objects/Enemy.hpp"
#include "Core/AppUtil.hpp"
#include "Objects/Player.hpp"
#include "Util/Logger.hpp"

Enemy::Enemy(int id)
    : Actor(id, "", true) {}

void Enemy::Reaction(std::shared_ptr<Player> player) {
  LOG_INFO("Entering battle! Enemy ID: {} ({})", m_object_id,
           AppUtil::GetIdString(m_object_id));
  // TODO: Battle logic and damage calculation

  TriggerReplacement(0); // Replace with empty floor
}
