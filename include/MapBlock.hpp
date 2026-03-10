#ifndef MAP_BLOCK_HPP
#define MAP_BLOCK_HPP

#include "AllObjects.hpp"
#include <string>

class MapBlock : public AllObjects {
public:
  MapBlock(int initialId = 0);
  ~MapBlock() override = default;

  void SetObjectId(int newId) override;

  glm::vec2 GetImageSize() const {
    if (m_Drawable) {
      return m_Drawable->GetSize();
    }
    return {0.0f, 0.0f};
  }

  // Data-driven properties
  bool IsPassable() const { return m_IsPassable; }
  void SetPassable(bool passable) { m_IsPassable = passable; }

private:
  bool m_IsPassable = true; // Default to passable, can be changed per instance
  std::string GetImagePath(int id) const;
  void UpdateProperties(int id);
};

#endif // MAP_BLOCK_HPP
