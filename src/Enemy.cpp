#include "Enemy.hpp"
#include "AppUtil.hpp"
#include "Util/Logger.hpp"

Enemy::Enemy(int id)
    : Entity(id,
             MAGIC_TOWER_RESOURCE_DIR "/bmp/Enemy/" +
                 AppUtil::GetIdResourcePath(id),
             true) {}

void Enemy::reaction() {
  LOG_INFO("Entering battle! Enemy ID: {} ({})", m_ObjectId,
           AppUtil::GetIdString(m_ObjectId));
  // TODO: Battle logic and damage calculation
  SetVisible(false);
}
