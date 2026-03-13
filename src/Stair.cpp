#include "Stair.hpp"
#include "AppUtil.hpp"
#include "Util/Logger.hpp"

Stair::Stair(int id, TriggerCallback callback)
    : Entity(id,
             MAGIC_TOWER_RESOURCE_DIR "/bmp/Stair/" +
                 AppUtil::GetIdResourcePath(id),
             true),
      m_OnTrigger(std::move(callback)) {}

bool Stair::IsPassable() const { return true; }

void Stair::reaction() {
  LOG_INFO("Using stairs... ID: {} ({})", m_ObjectId,
           AppUtil::GetIdString(m_ObjectId));
  if (m_OnTrigger) {
    if (m_ObjectId == 701) {
      m_OnTrigger(1); // Up
    } else if (m_ObjectId == 702) {
      m_OnTrigger(-1); // Down
    }
  }
}
