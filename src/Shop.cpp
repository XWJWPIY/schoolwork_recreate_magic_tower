#include "Shop.hpp"
#include "AppUtil.hpp"
#include "Util/Logger.hpp"

Shop::Shop(int id)
    : Entity(id,
             MAGIC_TOWER_RESOURCE_DIR "/bmp/Things/" +
                 AppUtil::GetIdString(id) + ".png",
             true) {}

void Shop::reaction() {
  LOG_INFO("Opening Shop menu... ID: {} ({})", m_ObjectId,
           AppUtil::GetIdString(m_ObjectId));
  // TODO: Implement trading system (UI/Logic)
}
