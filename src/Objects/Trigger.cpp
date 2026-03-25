#include "Objects/Trigger.hpp"
#include "Objects/Player.hpp"
#include "Core/AppUtil.hpp"
#include "Util/Logger.hpp"

Trigger::Trigger(int id, TriggerCallback callback)
    : Entity(id, AppUtil::GetFullResourcePath(id), true),
      m_trigger_callback(callback) {
}

void Trigger::Reaction(std::shared_ptr<Player> player) {
  LOG_INFO("Triggering event... ID: {} ({})", m_object_id,
           AppUtil::GetIdString(m_object_id));
  if (m_trigger_callback) {
      // Use the name from registry as the script path suffix
      m_trigger_callback(std::static_pointer_cast<Trigger>(shared_from_this()), 
                        AppUtil::GlobalObjectRegistry[m_object_id].name);
  }
}
