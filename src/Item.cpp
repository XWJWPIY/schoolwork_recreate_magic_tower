#include "Item.hpp"
#include "AppUtil.hpp"
#include "Player.hpp"
#include "Util/Logger.hpp"

Item::Item(int id)
    : Entity(id, "", true) {}

void Item::reaction(std::shared_ptr<Player> player) {
  LOG_INFO("Item collected! ID: {} ({})", m_ObjectId,
           AppUtil::GetIdString(m_ObjectId));
  
  if (player && m_ObjectId >= 201 && m_ObjectId <= 203) {
    player->AddKey(m_ObjectId);
  }

  if (m_ReplacementComp) {
    m_ReplacementComp->ReplaceWith(m_GridX, m_GridY, 0); // Replace with empty floor
  } else {
    SetVisible(false);
  }
}
