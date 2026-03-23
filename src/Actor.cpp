#include "Actor.hpp"
#include "AppUtil.hpp"

Actor::Actor(int initialId, const std::string &imagePath, bool canReact)
    : Entity(initialId, imagePath, canReact) {
  
  // Try to load combat properties if they exist in the registry
  auto it = AppUtil::GlobalObjectRegistry.find(initialId);
  if (it != AppUtil::GlobalObjectRegistry.end() && it->second.combat_props) {
      SetAttr(AppUtil::Effect::HP, it->second.combat_props->hp);
      SetAttr(AppUtil::Effect::ATTACK, it->second.combat_props->attack);
      SetAttr(AppUtil::Effect::DEFENSE, it->second.combat_props->defense);
      // The combat component defines exp_reward.
      SetAttr(AppUtil::Effect::EXP, it->second.combat_props->exp_reward);
  }
}
