#include "Entity.hpp"
#include "AppUtil.hpp"
#include "Util/Image.hpp"
#include <memory>
#include <filesystem>

Entity::Entity(int initialId, const std::string &imagePath, bool canReact)
    : AllObjects(std::make_shared<Util::Image>(
                     imagePath.empty() 
                        ? (MAGIC_TOWER_RESOURCE_DIR + AppUtil::GetIdResourcePath(initialId))
                        : (std::filesystem::exists(imagePath)
                           ? imagePath
                           : MAGIC_TOWER_RESOURCE_DIR "/bmp/Door/no_door.png")),
                 -2, initialId),
      m_grid_x(0), m_grid_y(0), m_can_react(canReact) {
  
  auto it = AppUtil::GlobalObjectRegistry.find(initialId);
  if (it != AppUtil::GlobalObjectRegistry.end()) {
      m_is_passable = it->second.is_passable;
  } else {
      m_is_passable = false; // Default for unknown entities
  }
  // Update properties after members are initialized
  UpdateProperties(initialId);
}

void Entity::SetObjectId(int newId) {
  m_object_id = newId;
  UpdateProperties(newId);
}

void Entity::UpdateProperties(int id) {
  // Basic shared properties (hp, attack, defense) logic could go here
  // Currently placeholder
}

void Entity::ObjectUpdate() {
  auto it = AppUtil::GlobalObjectRegistry.find(m_object_id);
  if (it != AppUtil::GlobalObjectRegistry.end() && it->second.animation_frames > 1) {
    // Current animated path
    std::string path = MAGIC_TOWER_RESOURCE_DIR + AppUtil::GetIdResourcePath(m_object_id);
    
    // Simple way to check if we need to update drawable
    // In a more complex system, we might store the current path or frame index
    m_Drawable = std::make_shared<Util::Image>(path);
  }
}
