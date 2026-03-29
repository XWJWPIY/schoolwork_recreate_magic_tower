#include "Objects/Stair.hpp"
#include "Core/AppUtil.hpp"
#include "Objects/Player.hpp"
#include "Util/Logger.hpp"

Stair::Stair(int id, TriggerCallback callback)
    : Entity(id, true),
      m_on_trigger(std::move(callback)) {}

void Stair::Reaction(std::shared_ptr<Player> player) {
    LOG_INFO("Using stairs... ID: {} ({})", m_object_id,
             AppUtil::GetIdString(m_object_id));
    
    if (m_on_trigger) {
        auto it = AppUtil::GlobalObjectRegistry.find(m_object_id);
        if (it != AppUtil::GlobalObjectRegistry.end()) {
            bool isRelative = it->second.GetBool("is_relative", true);
            int relation = it->second.GetInt("relation", 0);
            
            LOG_INFO("Stair Activation: ID {}, Relation {}, IsRelative {}", 
                     m_object_id, relation, isRelative);
            m_on_trigger(relation, isRelative);
        }
    }
}
