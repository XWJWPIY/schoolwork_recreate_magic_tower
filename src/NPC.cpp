#include "NPC.hpp"
#include "AppUtil.hpp"
#include "Player.hpp"
#include "Util/Logger.hpp"

NPC::NPC(int id)
    : Entity(id, "", true) {}

void NPC::Reaction(std::shared_ptr<Player> player) {
  LOG_INFO("Talking to NPC... ID: {} ({})", m_object_id,
           AppUtil::GetIdString(m_object_id));
  // TODO: Trigger dialogue system
}
