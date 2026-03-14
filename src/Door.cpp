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
  std::string doorName = AppUtil::GetIdString(id);
  std::vector<std::string> paths;
  for (int i = 1; i <= MAX_ANIMATION_FRAMES; i++) {
    std::string path = MAGIC_TOWER_RESOURCE_DIR "/bmp/Door/" + doorName +
                       std::to_string(i);
    // Handle the inconsistent extension case (some are .BMP, some .bmp)
    if (doorName == "iron_fence" && i == 2) {
      path += ".bmp";
    } else {
      path += ".BMP";
    }
    paths.push_back(path);
  }

  // Create Animation: paths, play, interval(ms), looping, cooldown
  m_Animation = std::make_shared<Util::Animation>(paths, false, 100, false);
  SetDrawable(m_Animation);
}

void Door::reaction(std::shared_ptr<Player> player) {
  if (m_Animation->GetState() == Util::Animation::State::PLAY)
    return;

  if (m_ObjectId == 301) { // Iron Fence
    LOG_INFO("Opening Iron Fence!");
    m_Animation->Play();
    m_CanReact = false;
    return;
  }

  if (player && player->UseKey(m_ObjectId)) {
    LOG_INFO("Opening Door! ID: {} ({})", m_ObjectId,
             AppUtil::GetIdString(m_ObjectId));
    m_Animation->Play();
    m_CanReact = false;
  } else if (m_ObjectId == 305) {
    LOG_INFO("Green door {} requires special condition.", AppUtil::GetIdString(m_ObjectId));
    // TODO: Implement special condition for Green Door
  } else {
    LOG_INFO("No key for door {} ({})", m_ObjectId,
             AppUtil::GetIdString(m_ObjectId));
  }
}

void Door::ObjectUpdate() {
  // If animation has ended
  if (m_Animation->GetState() == Util::Animation::State::ENDED) {
    if (m_ReplacementComp) {
      m_ReplacementComp->ReplaceWith(m_GridX, m_GridY, 0);
    } else {
      SetVisible(false);
    }
  }
}
