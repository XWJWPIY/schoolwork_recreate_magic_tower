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

  if (m_object_id == 304) { // Iron Fence (ID now 304)
    LOG_INFO("Opening Iron Fence!");
    m_animation->Play();
    m_can_react = false;
    return;
  }

  if (player && player->UseKey(m_object_id)) {
    LOG_INFO("Opening Door! ID: {} ({})", m_object_id,
             AppUtil::GetIdString(m_object_id));
    m_animation->Play();
    m_can_react = false;
  } else if (m_object_id == 305) {
    LOG_INFO("Green door {} requires special condition.", AppUtil::GetIdString(m_object_id));
    // TODO: Implement special condition for Green Door
  } else {
    LOG_INFO("No key for door {} ({})", m_object_id,
             AppUtil::GetIdString(m_object_id));
  }
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
