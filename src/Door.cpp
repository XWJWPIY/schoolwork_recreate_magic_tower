#include "Door.hpp"
#include "AppUtil.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include "Util/Image.hpp"

Door::Door(int id)
    : Entity(id,
             MAGIC_TOWER_RESOURCE_DIR "/bmp/Door/" +
                 AppUtil::GetIdString(id) + "1.BMP",
             true) {}

void Door::reaction() {
  if (m_IsOpening) return;

  LOG_INFO("Opening Door! ID: {} ({})", m_ObjectId,
           AppUtil::GetIdString(m_ObjectId));
  
  m_IsOpening = true;
  m_CanReact = false; // Prevent multiple reactions during animation
  m_AnimationFrame = 1;
  m_FrameTimer = 0.0f;
}

void Door::ObjectUpdate() {
  if (!m_IsOpening) return;

  m_FrameTimer += static_cast<float>(Util::Time::GetDeltaTime());

  if (m_FrameTimer >= FRAME_DELAY) {
    m_FrameTimer = 0.0f;
    m_AnimationFrame++;

    if (m_AnimationFrame <= MAX_ANIMATION_FRAMES) {
      // Update image frame
      std::string doorName = AppUtil::GetIdString(m_ObjectId);
      std::string imagePath = MAGIC_TOWER_RESOURCE_DIR "/bmp/Door/" + doorName + std::to_string(m_AnimationFrame);
      
      // Handle the inconsistent extension case (some are .BMP, some .bmp based on list_dir)
      if (doorName == "iron_fence" && m_AnimationFrame == 2) {
          imagePath += ".bmp";
      } else {
          imagePath += ".BMP";
      }

      auto image = std::dynamic_pointer_cast<Util::Image>(m_Drawable);
      if (image) {
        image->SetImage(imagePath);
      }
    } else {
      // Animation finished
      m_IsOpening = false;
      if (m_ReplacementComp) {
        m_ReplacementComp->ReplaceWith(m_GridX, m_GridY, 0);
      } else {
        SetVisible(false);
      }
    }
  }
}
