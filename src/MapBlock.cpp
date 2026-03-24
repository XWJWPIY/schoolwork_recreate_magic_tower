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
  if (m_object_id == newId)
    return;

  m_object_id = newId;
  UpdateProperties(m_object_id);
}

void MapBlock::ObjectUpdate() {
  auto it = AppUtil::GlobalObjectRegistry.find(m_object_id);
  if (it != AppUtil::GlobalObjectRegistry.end() && it->second.frames > 1) {
    int global_frame = AppUtil::TileAnimationManager::GetGlobalFrame2(500);
    if (m_current_local_frame != global_frame) {
      m_current_local_frame = global_frame;
      // Update image
      auto image = std::dynamic_pointer_cast<Util::Image>(m_Drawable);
      if (image) {
        std::string basePath = AppUtil::GetBaseImagePath(m_object_id);
        image->SetImage(AppUtil::GetPhaseImagePath(basePath, m_current_local_frame));
      }
    }
  }
}

void MapBlock::UpdateProperties(int id) {
  if (id == 0) {
    SetVisible(false);
    m_is_passable = true;
    // Load a standard tile (road) as size template for ID 0 but keep it hidden
    SetDrawable(std::make_shared<Util::Image>(AppUtil::GetStaticResourcePath("bmp/Road/road1.bmp")));
    return;
  }

  auto it = AppUtil::GlobalObjectRegistry.find(id);
  if (it == AppUtil::GlobalObjectRegistry.end()) {
    return;
  }

  const auto& meta = it->second;
  SetVisible(true);
  m_is_passable = meta.is_passable;
  
  // Use current global frame to initialize if it's an animated tile
  if (meta.frames > 1) {
    m_current_local_frame = AppUtil::TileAnimationManager::GetGlobalFrame2(500);
    std::string basePath = AppUtil::GetBaseImagePath(id);
    SetDrawable(std::make_shared<Util::Image>(AppUtil::GetPhaseImagePath(basePath, m_current_local_frame)));
  } else {
    SetDrawable(std::make_shared<Util::Image>(AppUtil::GetFullResourcePath(id)));
  }
}

std::string MapBlock::GetImagePath(int id) const {
    return AppUtil::GetFullResourcePath(id);
}
