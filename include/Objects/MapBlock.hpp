#ifndef MAP_BLOCK_HPP
#define MAP_BLOCK_HPP

#include "Objects/Entity.hpp"
#include <vector>

class MapBlock : public Entity {
public:
  MapBlock(int initialId = 0);
  ~MapBlock() override = default;

  glm::vec2 GetImageSize() const {
    if (this->m_Drawable) {
      return this->m_Drawable->GetSize();
    }
    return {0.0f, 0.0f};
  }
};

#endif // MAP_BLOCK_HPP
