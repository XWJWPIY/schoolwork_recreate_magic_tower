#include "Entity.hpp"

#include "Util/Image.hpp"
#include <filesystem>

Entity::Entity(int initialId, const std::string &imagePath, bool canReact)
    : AllObjects(std::make_unique<Util::Image>(
                     std::filesystem::exists(imagePath)
                         ? imagePath
                         : MAGIC_TOWER_RESOURCE_DIR "/bmp/Door/no_door.png"),
                 -2, initialId),
      x(0), y(0), m_CanReact(canReact) {
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
