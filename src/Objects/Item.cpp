#include "Objects/Item.hpp"
#include "Core/AppUtil.hpp"
#include "Objects/Player.hpp"
#include "Util/Logger.hpp"

Item::Item(int id, NoticeCallback callback)
    : Entity(id, true), m_notice_callback(std::move(callback)) {}

void Item::Reaction(std::shared_ptr<Player> player) {
  LOG_INFO("Item collected! ID: {} ({})", m_object_id,
           AppUtil::GetIdString(m_object_id));
  
  if (player) {
    auto it = AppUtil::GlobalObjectRegistry.find(m_object_id);
    if (it != AppUtil::GlobalObjectRegistry.end()) {
      const auto &meta = it->second;
      
      // Trigger dialog notice if available
      std::string dialog = meta.GetString(AppUtil::Attr::DIALOG);
      if (m_notice_callback && !dialog.empty()) {
        m_notice_callback(dialog);
      }

      // Apply all discovered attributes as relative effects
      ForEachAttribute([player](AppUtil::Effect eff, int val) {
          player->ApplyEffect(eff, val);
      });
    }
  }

  TriggerReplacement(0); // Replace with empty floor
}
