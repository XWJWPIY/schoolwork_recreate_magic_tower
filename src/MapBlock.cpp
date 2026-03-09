#include "MapBlock.hpp"
#include "AppUtil.hpp"
#include "Util/Image.hpp"
#include <memory>

MapBlock::MapBlock(int initialId)
    // Initialize GameObject with default image and deep Z-Index.
    : GameObject(std::make_unique<Util::Image>(GetImagePath(initialId)), -5) {
  SetBlockId(initialId);
}

void MapBlock::SetBlockId(int newId) {
  if (m_CurrentId == newId)
    return;

  m_CurrentId = newId;
  auto temp = std::dynamic_pointer_cast<Util::Image>(m_Drawable);
  if (temp) {
    temp->SetImage(GetImagePath(m_CurrentId));
  }
}

std::string MapBlock::GetImagePath(int id) const {
  switch (id) {
  case 0:
    return MAGIC_TOWER_RESOURCE_DIR "/bmp/Road/road.bmp";
  case 1:
    return MAGIC_TOWER_RESOURCE_DIR "/bmp/Road/lava_road.bmp";
  case 2:
    return MAGIC_TOWER_RESOURCE_DIR "/bmp/Road/wall.bmp";
  case 3:
    return MAGIC_TOWER_RESOURCE_DIR "/bmp/Road/wall_b.bmp";
  case 4:
    return MAGIC_TOWER_RESOURCE_DIR "/bmp/Road/wall_shine.bmp";
  case 5:
    return MAGIC_TOWER_RESOURCE_DIR "/bmp/Road/wall_special.bmp";
  case 6:
    return MAGIC_TOWER_RESOURCE_DIR "/bmp/Road/lava.bmp";
  default:
    return MAGIC_TOWER_RESOURCE_DIR "/bmp/Road/road.bmp";
  }
}
