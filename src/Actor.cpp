#include "Actor.hpp"
#include "AppUtil.hpp"

Actor::Actor(int initialId, const std::string &imagePath, bool canReact)
    : Entity(initialId, imagePath, canReact) {
  
  // Try to load combat properties if they exist in the registry
  auto it = AppUtil::GlobalObjectRegistry.find(initialId);
  if (it != AppUtil::GlobalObjectRegistry.end() && it->second.combat_props) {
      m_hp = it->second.combat_props->hp;
      m_attack = it->second.combat_props->attack;
      m_defense = it->second.combat_props->defense;
      // The combat component defines exp_reward. For Player, this gets overwritten.
      m_exp = it->second.combat_props->exp_reward;
  }
}
