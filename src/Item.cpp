#include "Item.hpp"
#include "AppUtil.hpp"
#include "Player.hpp"
#include "Util/Logger.hpp"

Item::Item(int id, NoticeCallback callback)
    : Entity(id, "", true), m_notice_callback(std::move(callback)) {}

void Item::Reaction(std::shared_ptr<Player> player) {
  LOG_INFO("Item collected! ID: {} ({})", m_object_id,
           AppUtil::GetIdString(m_object_id));
  
  if (player) {
    auto it = AppUtil::GlobalObjectRegistry.find(m_object_id);
    if (it != AppUtil::GlobalObjectRegistry.end()) {
      const auto &meta = it->second;
      
      // Trigger dialog notice if available
      if (m_notice_callback && meta.dialog_props && !meta.dialog_props->lines.empty()) {
        m_notice_callback(meta.dialog_props->lines[0]);
      }

      if (meta.item_props) {
        for (const auto &effect : meta.item_props->effects) {
          player->ApplyEffect(AppUtil::AttributeRegistry::ToEffect(effect.type_id), effect.value);
        }
      }
    }
  }

  TriggerReplacement(0); // Replace with empty floor
}
