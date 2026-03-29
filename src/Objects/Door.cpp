#include "Objects/Door.hpp"
#include "Objects/Player.hpp"
#include "Core/AppUtil.hpp"
#include "Util/Logger.hpp"

Door::Door(int id)
    : Entity(id, true) {
  SetupAnimation(id, false, 100); // looping=false, 100ms/frame, one-shot
}

bool Door::CheckCondition(std::shared_ptr<Player> player) const {
  if (!player) return false;

  auto it = AppUtil::GlobalObjectRegistry.find(m_object_id);
  if (it == AppUtil::GlobalObjectRegistry.end()) return true;

  const auto &meta = it->second;
  if (meta.GetBool(AppUtil::Attr::IS_PASSIVE, false)) {
    return false;
  }

  bool can_open = true;
  ForEachAttribute([&](AppUtil::Effect eff, int val) {
    if (!player->MeetsRequirement(eff, val)) {
      can_open = false;
    }
  });

  return can_open;
}

void Door::Reaction(std::shared_ptr<Player> player) {
  if (m_animation && m_animation->GetState() == Util::Animation::State::PLAY)
    return;

  // Since CheckCondition is now called before Reaction in Player::Move,
  // we can assume conditions are met, but we still perform the consumption here.
  
  // Consume resources (keys)
  ForEachAttribute([&](AppUtil::Effect eff, int val) {
    player->ApplyEffect(eff, -val);
  });

  LOG_INFO("Opening Door! ID: {} ({})", m_object_id, AppUtil::GetIdString(m_object_id));
  m_animation->Play();
  m_can_react = false;
}

void Door::ObjectUpdate() {
  // Pure business logic: destroy self after animation ends
  if (m_animation && m_animation->GetState() == Util::Animation::State::ENDED) {
    TriggerReplacement(0);
  }
}
