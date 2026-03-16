#include "Stair.hpp"
#include "AppUtil.hpp"
#include "Player.hpp"
#include "Util/Logger.hpp"

Stair::Stair(int id, TriggerCallback callback)
    : Entity(id, "", true),
      m_on_trigger(std::move(callback)) {}

void Stair::Reaction(std::shared_ptr<Player> player) {
  LOG_INFO("Using stairs... ID: {} ({})", m_object_id,
           AppUtil::GetIdString(m_object_id));
  if (m_on_trigger) {
    if (m_object_id == 701) {
      m_on_trigger(1); // Up
    } else if (m_object_id == 702) {
      m_on_trigger(-1); // Down
    }
  }
}
