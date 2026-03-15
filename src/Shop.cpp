#include "Shop.hpp"
#include "AppUtil.hpp"
#include "Player.hpp"
#include "Util/Logger.hpp"

Shop::Shop(int id)
    : Entity(id, "", true) {}

void Shop::reaction(std::shared_ptr<Player> player) {
  LOG_INFO("Opening Shop menu... ID: {} ({})", m_ObjectId,
           AppUtil::GetIdString(m_ObjectId));
  // TODO: Implement trading system (UI/Logic)
}
