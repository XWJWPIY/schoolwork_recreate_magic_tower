#include "Door.hpp"
#include "AppUtil.hpp"
#include "Util/Logger.hpp"

Door::Door(int id)
    : Entity(id,
             MAGIC_TOWER_RESOURCE_DIR "/bmp/Door/" +
                 AppUtil::GetIdResourcePath(id),
             true) {}

void Door::reaction() {
  LOG_INFO("Interacting with Door! ID: {} ({})", m_ObjectId,
           AppUtil::GetIdString(m_ObjectId));
  // TODO: Check for keys
  SetVisible(false);
}
