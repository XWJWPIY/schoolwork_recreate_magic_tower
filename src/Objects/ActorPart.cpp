#include "Objects/ActorPart.hpp"
#include "Core/AppUtil.hpp"
#include "Objects/Player.hpp"
#include "Util/Logger.hpp"

ActorPart::ActorPart(int id) : Entity(id, true) {}

bool ActorPart::CheckCondition(std::shared_ptr<Player> player) const {
  auto it = AppUtil::GlobalObjectRegistry.find(m_object_id);
  if (it == AppUtil::GlobalObjectRegistry.end()) return false;
  int core_id = it->second.GetInt("Core_ID", 0);
  if (m_replacement_comp && core_id > 0) {
    auto core = m_replacement_comp->FindEntityById(core_id);
    if (core) {
      return core->CheckCondition(player);
    }
  }
  return false;
}

void ActorPart::Reaction(std::shared_ptr<Player> player) {
  auto it = AppUtil::GlobalObjectRegistry.find(m_object_id);
  if (it == AppUtil::GlobalObjectRegistry.end()) return;
  int core_id = it->second.GetInt("Core_ID", 0);
  if (m_replacement_comp && core_id > 0) {
    auto core = m_replacement_comp->FindEntityById(core_id);
    if (core) {
      core->Reaction(player);
    } else {
      LOG_ERROR("ActorPart {} cannot find Core_ID {}", m_object_id, core_id);
    }
  }
}
