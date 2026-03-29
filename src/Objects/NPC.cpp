#include "Objects/NPC.hpp"
#include "Objects/Player.hpp"
#include "Core/AppUtil.hpp"
#include "Util/Logger.hpp"

NPC::NPC(int id, TalkCallback callback)
    : Entity(id, true), m_talk_callback(callback) {}

void NPC::Reaction(std::shared_ptr<Player> player) {
  LOG_INFO("Talking to NPC... ID: {} ({})", m_object_id,
           AppUtil::GetIdString(m_object_id));
  if (m_talk_callback) {
      m_talk_callback(std::static_pointer_cast<NPC>(shared_from_this()), 
                      AppUtil::GlobalObjectRegistry[m_object_id].name);
  }
}
