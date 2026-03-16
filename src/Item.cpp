#include "Item.hpp"
#include "AppUtil.hpp"
#include "Player.hpp"
#include "Util/Logger.hpp"

Item::Item(int id)
    : Entity(id, "", true) {}

void Item::Reaction(std::shared_ptr<Player> player) {
  LOG_INFO("Item collected! ID: {} ({})", m_object_id,
           AppUtil::GetIdString(m_object_id));
  
  if (player && m_object_id >= 201 && m_object_id <= 203) {
    player->AddKey(m_object_id);
  }

  if (m_replacement_comp) {
    m_replacement_comp->ReplaceWith(m_grid_x, m_grid_y, 0); // Replace with empty floor
  } else {
    SetVisible(false);
  }
}
