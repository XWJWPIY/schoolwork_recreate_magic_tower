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
  // ID 0 is now reserved for "Empty/Transparent"
  if (id == 0) {
    SetVisible(false);
    m_IsPassable = true;
    return;
  }

  SetVisible(true);
  // Simple table for passability based on ID (can be moved to CSV later)
  if (id == 1 || id == 2) { // 1: road, 2: lava_road
    m_IsPassable = true;
  } else {
    m_IsPassable = false; // 3, 4, 5, 6 are walls, 7 is lava
  }
}

std::string MapBlock::GetImagePath(int id) const {
  switch (id) {
  case 1:
    return MAGIC_TOWER_RESOURCE_DIR "/bmp/Road/road.bmp";
  case 2:
    return MAGIC_TOWER_RESOURCE_DIR "/bmp/Road/lava_road.bmp";
  case 3:
    return MAGIC_TOWER_RESOURCE_DIR "/bmp/Road/wall.bmp";
  case 4:
    return MAGIC_TOWER_RESOURCE_DIR "/bmp/Road/wall_b.bmp";
  case 5:
    return MAGIC_TOWER_RESOURCE_DIR "/bmp/Road/wall_shine.bmp";
  case 6:
    return MAGIC_TOWER_RESOURCE_DIR "/bmp/Road/wall_special.bmp";
  case 7:
    return MAGIC_TOWER_RESOURCE_DIR "/bmp/Road/lava.bmp";
  case 0:
  default:
    return MAGIC_TOWER_RESOURCE_DIR "/bmp/Road/road.bmp";
  }
}
