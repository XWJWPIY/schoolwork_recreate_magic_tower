#include "Door.hpp"
#include "AppUtil.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include "Util/Image.hpp"
#include "Player.hpp"

Door::Door(int id)
    : Entity(id,
             MAGIC_TOWER_RESOURCE_DIR "/bmp/Door/" +
                 AppUtil::GetIdString(id) + "1.BMP",
             true) {
  std::string door_name = AppUtil::GetIdString(id);
  std::vector<std::string> paths;
  for (int i = 1; i <= MAX_ANIMATION_FRAMES; i++) {
    std::string path = MAGIC_TOWER_RESOURCE_DIR "/bmp/Door/" + door_name +
                       std::to_string(i);
    // Handle the inconsistent extension case (some are .BMP, some .bmp)
    if (door_name == "iron_fence" && i == 2) {
      path += ".bmp";
    } else {
      path += ".BMP";
    }
    paths.push_back(path);
  }

  // Create Animation: paths, play, interval(ms), looping, cooldown
  m_animation = std::make_shared<Util::Animation>(paths, false, 100, false);
  SetDrawable(m_animation);
}

void Door::Reaction(std::shared_ptr<Player> player) {
  if (m_animation->GetState() == Util::Animation::State::PLAY)
    return;

  auto it = AppUtil::GlobalObjectRegistry.find(m_object_id);
  if (it == AppUtil::GlobalObjectRegistry.end() || !it->second.door_props) {
    LOG_INFO("No door metadata for ID {}", m_object_id);
    return;
  }

  const auto &meta = it->second;
  const auto &props = *meta.door_props;

  if (props.is_passive) {
    LOG_INFO("Door {} is passive and requires a special condition to open.",
             meta.name);
    return;
  }

  // Open immediately if no keys are required
  if (props.yellow_key == 0 && props.blue_key == 0 && props.red_key == 0) {
    LOG_INFO("Opening {} (requires no keys)!", meta.name);
    m_animation->Play();
    m_can_react = false;
    return;
  }

  bool can_open = true;
  if (props.yellow_key > 0) {
      if (player->GetAttr(AppUtil::Effect::KEY_YELLOW) >= props.yellow_key) {
          player->ApplyEffect(AppUtil::Effect::KEY_YELLOW, -props.yellow_key);
      } else {
          can_open = false;
      }
  }
  if (can_open && props.blue_key > 0) {
      if (player->GetAttr(AppUtil::Effect::KEY_BLUE) >= props.blue_key) {
          player->ApplyEffect(AppUtil::Effect::KEY_BLUE, -props.blue_key);
      } else {
          can_open = false;
      }
  }
  if (can_open && props.red_key > 0) {
      if (player->GetAttr(AppUtil::Effect::KEY_RED) >= props.red_key) {
          player->ApplyEffect(AppUtil::Effect::KEY_RED, -props.red_key);
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
  // If animation has ended
  if (m_animation->GetState() == Util::Animation::State::ENDED) {
    if (m_replacement_comp) {
      m_replacement_comp->ReplaceWith(m_grid_x, m_grid_y, 0);
    } else {
      SetVisible(false);
    }
  }
}
