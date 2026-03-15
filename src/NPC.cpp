#include "NPC.hpp"
#include "AppUtil.hpp"
#include "Player.hpp"
#include "Util/Logger.hpp"

NPC::NPC(int id)
    : Entity(id, "", true) {}

void NPC::reaction(std::shared_ptr<Player> player) {
  LOG_INFO("Talking to NPC... ID: {} ({})", m_ObjectId,
           AppUtil::GetIdString(m_ObjectId));
  // TODO: Trigger dialogue system
}
