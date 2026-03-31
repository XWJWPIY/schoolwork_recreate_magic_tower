#include "Objects/ActorPart.hpp"
#include "Core/AppUtil.hpp"
#include "Objects/Player.hpp"
#include "Util/Logger.hpp"

ActorPart::ActorPart(int id) : Entity(id, true) {}

bool ActorPart::CheckCondition(std::shared_ptr<Player> player) const {
  auto meta = AppUtil::GlobalObjectRegistry[m_object_id];
  int core_id = meta.GetInt("Core_ID", 0);
  if (m_replacement_comp && core_id > 0) {
    auto core = m_replacement_comp->FindEntityById(core_id);
    if (core) {
      return core->CheckCondition(player);
    }
  }
  return false;
}

void ActorPart::Reaction(std::shared_ptr<Player> player) {
  auto meta = AppUtil::GlobalObjectRegistry[m_object_id];
  int core_id = meta.GetInt("Core_ID", 0);
  if (m_replacement_comp && core_id > 0) {
    auto core = m_replacement_comp->FindEntityById(core_id);
    if (core) {
      core->Reaction(player);
    } else {
      LOG_ERROR("ActorPart {} cannot find Core_ID {}", m_object_id, core_id);
    }
  }
}
