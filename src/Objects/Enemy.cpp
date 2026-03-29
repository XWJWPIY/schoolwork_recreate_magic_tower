#include "Objects/Enemy.hpp"
#include "Core/AppUtil.hpp"
#include "Objects/Player.hpp"
#include "Util/Logger.hpp"

Enemy::Enemy(int id)
    : Actor(id, true) {}

void Enemy::Reaction(std::shared_ptr<Player> player) {
  LOG_INFO("Entering battle! Enemy ID: {} ({})", m_object_id,
           AppUtil::GetIdString(m_object_id));

  if (m_start_battle_cb) {
      m_start_battle_cb(std::dynamic_pointer_cast<Enemy>(shared_from_this()));
  } else {
      OnDefeated(player); // Fallback
  }
}

void Enemy::OnDefeated(std::shared_ptr<Player> player) {
  auto meta = AppUtil::GlobalObjectRegistry[m_object_id];
  
  if (player) {
      player->ApplyEffect(AppUtil::Effect::EXP, meta.GetInt(AppUtil::Attr::EXP));
      player->ApplyEffect(AppUtil::Effect::COIN, meta.GetInt(AppUtil::Attr::COIN));
  }

  int w = meta.GetInt("Boss_Width", 1);
  int h = meta.GetInt("Boss_Height", 1);
  
  if (w > 1 || h > 1) {
    if (m_replacement_comp) {
      for (int dy = 0; dy < h; ++dy) {
        for (int dx = 0; dx < w; ++dx) {
          m_replacement_comp->ReplaceWith(m_grid_x + dx, m_grid_y + dy, 0);
        }
      }
    }
  } else {
    TriggerReplacement(0); // Replace with empty floor
  }
}

EnemyPart::EnemyPart(int id) : Entity(id, true) {}

bool EnemyPart::CheckCondition(std::shared_ptr<Player> player) const {
  auto meta = AppUtil::GlobalObjectRegistry[m_object_id];
  int core_id = meta.GetInt("Core_ID", 0);
  if (m_replacement_comp && core_id > 0) {
    auto core = m_replacement_comp->FindEntityById(core_id);
    if (core) {
      return core->CheckCondition(player);
    }
  }
  return false;
}

void EnemyPart::Reaction(std::shared_ptr<Player> player) {
  auto meta = AppUtil::GlobalObjectRegistry[m_object_id];
  int core_id = meta.GetInt("Core_ID", 0);
  if (m_replacement_comp && core_id > 0) {
    auto core = m_replacement_comp->FindEntityById(core_id);
    if (core) {
      core->Reaction(player);
    } else {
      LOG_ERROR("EnemyPart {} cannot find Core_ID {}", m_object_id, core_id);
    }
  }
}
