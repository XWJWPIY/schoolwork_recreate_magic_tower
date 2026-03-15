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
      m_GridX(0), m_GridY(0), m_CanReact(canReact) {
  
  auto it = AppUtil::GlobalObjectRegistry.find(initialId);
  if (it != AppUtil::GlobalObjectRegistry.end()) {
      m_IsPassable = it->second.isPassable;
  } else {
      m_IsPassable = false; // Default for unknown entities
  }
  // Update properties after members are initialized
  UpdateProperties(initialId);
}

void Entity::SetObjectId(int newId) {
  m_ObjectId = newId;
  UpdateProperties(newId);
}

void Entity::UpdateProperties(int id) {
  // Basic shared properties (hp, attack, defense) logic could go here
  // Currently placeholder
}
