#include "MapBlock.hpp"
#include "AppUtil.hpp"
#include "Util/Image.hpp"
#include <memory>
#include <string>

MapBlock::MapBlock(int initialId)
    // Initialize AllObjects with deep Z-Index.
    : AllObjects(nullptr, -5, initialId) {
  UpdateProperties(initialId);
}

void MapBlock::SetObjectId(int newId) {
  if (m_ObjectId == newId)
    return;

  m_ObjectId = newId;
  UpdateProperties(m_ObjectId);
}

void MapBlock::ObjectUpdate() {
  // Sync Animation (ID 2: lava_road, 5: wall_shine, 7: lava)
  if (m_ObjectId == 2 || m_ObjectId == 5 || m_ObjectId == 7) {
    int globalFrame = AppUtil::TileAnimationManager::GetGlobalFrame2(500);
    if (m_CurrentLocalFrame != globalFrame) {
      m_CurrentLocalFrame = globalFrame;
      // Update image
      auto image = std::dynamic_pointer_cast<Util::Image>(m_Drawable);
      if (image) {
        std::string base = GetImagePath(m_ObjectId);
        std::string prefix = base.substr(0, base.length() - 5);
        image->SetImage(prefix + std::to_string(m_CurrentLocalFrame) + ".bmp");
      }
    }
  }
}

void MapBlock::UpdateProperties(int id) {
  // ID 0 is now reserved for "Empty/Transparent"
  if (id == 0) {
    SetVisible(false);
    m_IsPassable = true;
    SetDrawable(std::make_shared<Util::Image>(GetImagePath(0)));
    return;
  }

  SetVisible(true);
  
  // Use current global frame to initialize if it's an animated tile
  if (id == 2 || id == 5 || id == 7) {
    m_CurrentLocalFrame = AppUtil::TileAnimationManager::GetGlobalFrame2(500);
    std::string base = GetImagePath(id);
    std::string prefix = base.substr(0, base.length() - 5);
    SetDrawable(std::make_shared<Util::Image>(prefix + std::to_string(m_CurrentLocalFrame) + ".bmp"));
  } else {
    SetDrawable(std::make_shared<Util::Image>(GetImagePath(id)));
  }

  // Simple table for passability based on ID
  if (id == 1 || id == 2) { // 1: road, 2: lava_road
    m_IsPassable = true;
  } else {
    m_IsPassable = false; // 3, 4, 5, 6 are walls, 7 is lava
  }
}

std::string MapBlock::GetImagePath(int id) const {
  switch (id) {
  case 1:
    return MAGIC_TOWER_RESOURCE_DIR "/bmp/Road/road1.bmp";
  case 2:
    return MAGIC_TOWER_RESOURCE_DIR "/bmp/Road/lava_road1.bmp";
  case 3:
    return MAGIC_TOWER_RESOURCE_DIR "/bmp/Road/wall1.bmp";
  case 4:
    return MAGIC_TOWER_RESOURCE_DIR "/bmp/Road/wall_b1.bmp";
  case 5:
    return MAGIC_TOWER_RESOURCE_DIR "/bmp/Road/wall_shine1.bmp";
  case 6:
    return MAGIC_TOWER_RESOURCE_DIR "/bmp/Road/wall_special1.bmp";
  case 7:
    return MAGIC_TOWER_RESOURCE_DIR "/bmp/Road/lava1.bmp";
  case 0:
  default:
    return MAGIC_TOWER_RESOURCE_DIR "/bmp/Road/road1.bmp";
  }
}
