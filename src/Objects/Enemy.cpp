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
  auto it = AppUtil::GlobalObjectRegistry.find(m_object_id);
  if (it == AppUtil::GlobalObjectRegistry.end()) return;
  const auto& meta = it->second;
  
  if (player) {
      player->ApplyEffect(AppUtil::Effect::EXP, meta.GetInt(AppUtil::Attr::EXP));
      player->ApplyEffect(AppUtil::Effect::COIN, meta.GetInt(AppUtil::Attr::COIN));
  }

  int w = meta.GetInt("Width", 1);
  int h = meta.GetInt("Height", 1);
  int nextEnemyId = meta.GetInt("Next_Enemy", 0);
  
  if (w > 1 || h > 1) {
    if (m_replacement_comp) {
      for (int dy = 0; dy < h; ++dy) {
        for (int dx = 0; dx < w; ++dx) {
          // If there is a next enemy, only spawn it at the origin (top-left) of the boss
          int replacement = (dx == 0 && dy == 0) ? nextEnemyId : 0;
          m_replacement_comp->ReplaceWith(m_grid_x + dx, m_grid_y + dy, replacement);
        }
      }
    }
  } else {
    TriggerReplacement(nextEnemyId); // Replace with nextEnemyId (default 0)
  }

  // Handle Reward Layer
  std::string rewardMap = meta.GetString("Reward_Map");
  if (!rewardMap.empty() && rewardMap != "0") {
      LOG_INFO("Enemy: Triggering reward map: {}", rewardMap);
      if (m_load_reward_layer_cb) {
          m_load_reward_layer_cb(rewardMap);
      }
  }
}

