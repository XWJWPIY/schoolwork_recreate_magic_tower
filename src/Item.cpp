#include "Item.hpp"
#include "AppUtil.hpp"
#include "Util/Logger.hpp"

Item::Item(int id)
    : Entity(id,
             MAGIC_TOWER_RESOURCE_DIR "/bmp/Item/" +
                 AppUtil::GetIdResourcePath(id),
             true) {}

void Item::reaction() {
  LOG_INFO("Item collected! ID: {} ({})", m_ObjectId,
           AppUtil::GetIdString(m_ObjectId));
  // TODO: Update player stats or inventory
  SetVisible(false);
}
