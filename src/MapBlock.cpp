#include "MapBlock.hpp"
#include "AppUtil.hpp"
#include "Util/Image.hpp"
#include <memory>
#include <string>

MapBlock::MapBlock(int initialId)
    // Initialize AllObjects with default image and deep Z-Index.
    : AllObjects(std::make_unique<Util::Image>(GetImagePath(initialId)), -5,
                 initialId) {
  UpdateProperties(initialId);
}

void MapBlock::SetObjectId(int newId) {
  if (m_ObjectId == newId)
    return;

  m_ObjectId = newId;
  auto temp = std::dynamic_pointer_cast<Util::Image>(m_Drawable);
  if (temp) {
    temp->SetImage(GetImagePath(m_ObjectId));
  }
  UpdateProperties(m_ObjectId);
}

void MapBlock::UpdateProperties(int id) {
  // Simple table for passability based on ID (can be moved to CSV later)
  if (id == 0 || id == 1) { // 0: road, 1: lava_road
    m_IsPassable = true;
  } else {
    m_IsPassable = false; // 2, 3, 4, 5 are walls, 6 is lava
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
