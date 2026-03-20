#include "Shop.hpp"
#include "AppUtil.hpp"
#include "Player.hpp"
#include "Util/Logger.hpp"

Shop::Shop(int id)
    : Entity(id, "", true) {}

void Shop::Reaction(std::shared_ptr<Player> player) {
  LOG_INFO("Opening Shop menu... ID: {} ({})", m_object_id,
           AppUtil::GetIdString(m_object_id));
  player->SetPendingShop(m_object_id);
}
