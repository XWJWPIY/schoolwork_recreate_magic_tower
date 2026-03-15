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
  auto it = AppUtil::GlobalObjectRegistry.find(m_ObjectId);
  if (it != AppUtil::GlobalObjectRegistry.end() && it->second.isAnimated) {
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
    SetDrawable(std::make_shared<Util::Image>(MAGIC_TOWER_RESOURCE_DIR + AppUtil::GetIdResourcePath(0)));
    return;
  }

  auto it = AppUtil::GlobalObjectRegistry.find(id);
  if (it == AppUtil::GlobalObjectRegistry.end()) {
    return;
  }

  const auto& meta = it->second;
  SetVisible(true);
  m_IsPassable = meta.isPassable;
  
  // Use current global frame to initialize if it's an animated tile
  if (meta.isAnimated) {
    m_CurrentLocalFrame = AppUtil::TileAnimationManager::GetGlobalFrame2(500);
    std::string base = MAGIC_TOWER_RESOURCE_DIR + AppUtil::GetIdResourcePath(id);
    std::string prefix = base.substr(0, base.length() - 5);
    SetDrawable(std::make_shared<Util::Image>(prefix + std::to_string(m_CurrentLocalFrame) + ".bmp"));
  } else {
    SetDrawable(std::make_shared<Util::Image>(MAGIC_TOWER_RESOURCE_DIR + AppUtil::GetIdResourcePath(id)));
  }
}

std::string MapBlock::GetImagePath(int id) const {
    return MAGIC_TOWER_RESOURCE_DIR + AppUtil::GetIdResourcePath(id);
}
