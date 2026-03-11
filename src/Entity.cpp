#include "Entity.hpp"

#include "Util/Image.hpp"

Entity::Entity(int initialId)
    : AllObjects(std::make_unique<Util::Image>(GetImagePath(initialId)), -2,
                 initialId),
      x(0), y(0) {
  UpdateProperties(initialId);
}

void Entity::SetObjectId(int newId) {
  if (m_ObjectId == newId)
    return;

  m_ObjectId = newId;
  auto temp = std::dynamic_pointer_cast<Util::Image>(m_Drawable);
  if (temp) {
    temp->SetImage(GetImagePath(m_ObjectId));
  }
  UpdateProperties(m_ObjectId);
}

void Entity::UpdateProperties(int id) {
  // If id is 0, we don't want to show anything, but the object exists.
  if (id == 0) {
    SetVisible(false);
    is_movable = false;
    return;
  }

  SetVisible(true);
  // Template for data-driven properties
  if (id == 303) {
    is_movable = true;
    hp = 1000;
    attack = 10;
    defense = 10;
  }
}

std::string Entity::GetImagePath(int id) const {
  switch (id) {
  case 303:
    return MAGIC_TOWER_RESOURCE_DIR "/bmp/Player/player_backward.png";
  case 0:
  default:
    // For ID 0, use a default but it will be hidden by UpdateProperties
    return MAGIC_TOWER_RESOURCE_DIR "/bmp/Road/road.bmp";
  }
}

bool Entity::canMove(int delta_x, int delta_y) {
  if (!is_movable)
    return false;

  // TODO: Add collision checking with Tile map
  return true;
}
