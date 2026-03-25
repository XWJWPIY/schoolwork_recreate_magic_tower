#include "Objects/Door.hpp"
#include "Objects/Player.hpp"
#include "Core/AppUtil.hpp"
#include "Util/Logger.hpp"

Door::Door(int id)
    : Entity(id, "", true) {
  SetupAnimation(id, false, 100); // looping=false, 100ms/frame, one-shot
}

void Door::Reaction(std::shared_ptr<Player> player) {
  if (m_animation && m_animation->GetState() == Util::Animation::State::PLAY)
    return;

  auto it = AppUtil::GlobalObjectRegistry.find(m_object_id);
  const auto &meta = it->second;

  if (meta.GetBool(AppUtil::Attr::IS_PASSIVE, false)) {
    LOG_INFO("Door {} is passive and requires a special condition to open.",
             meta.name);
    return;
  }

  int yellow_key = meta.GetInt(AppUtil::Attr::YELLOW_KEY);
  int blue_key = meta.GetInt(AppUtil::Attr::BLUE_KEY);
  int red_key = meta.GetInt(AppUtil::Attr::RED_KEY);

  // Open immediately if no keys are required
  if (yellow_key == 0 && blue_key == 0 && red_key == 0) {
    LOG_INFO("Opening {} (requires no keys)!", meta.name);
    m_animation->Play();
    m_can_react = false;
    return;
  }

  bool can_open = true;
  if (yellow_key > 0) {
      if (player->GetAttr(AppUtil::Effect::KEY_YELLOW) >= yellow_key) {
          player->ApplyEffect(AppUtil::Effect::KEY_YELLOW, -yellow_key);
      } else {
          can_open = false;
      }
  }
  if (can_open && blue_key > 0) {
      if (player->GetAttr(AppUtil::Effect::KEY_BLUE) >= blue_key) {
          player->ApplyEffect(AppUtil::Effect::KEY_BLUE, -blue_key);
      } else {
          can_open = false;
      }
  }
  if (can_open && red_key > 0) {
      if (player->GetAttr(AppUtil::Effect::KEY_RED) >= red_key) {
          player->ApplyEffect(AppUtil::Effect::KEY_RED, -red_key);
      } else {
          can_open = false;
      }
  }

  if (can_open) {
    LOG_INFO("Opening Door! ID: {} ({})", m_object_id, meta.name);
    m_animation->Play();
    m_can_react = false;
    return;
  }

  LOG_INFO("No key/condition for door {} ({})", m_object_id,
           AppUtil::GetIdString(m_object_id));
}

void Door::ObjectUpdate() {
  // Pure business logic: destroy self after animation ends
  if (m_animation && m_animation->GetState() == Util::Animation::State::ENDED) {
    TriggerReplacement(0);
  }
}
