#include "NPC.hpp"
#include "AppUtil.hpp"
#include "Util/Logger.hpp"

NPC::NPC(int id)
    : Entity(id,
             MAGIC_TOWER_RESOURCE_DIR "/bmp/NPC/" +
                 AppUtil::GetIdResourcePath(id),
             true) {}

void NPC::reaction() {
  LOG_INFO("Talking to NPC... ID: {} ({})", m_ObjectId,
           AppUtil::GetIdString(m_ObjectId));
  // TODO: Trigger dialogue system
}
