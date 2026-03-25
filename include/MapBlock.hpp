#ifndef MAP_BLOCK_HPP
#define MAP_BLOCK_HPP

#include "AllObjects.hpp"
#include "Util/Image.hpp"
#include <vector>

class MapBlock : public AllObjects {
public:
  MapBlock(int initialId = 0);
  ~MapBlock() override = default;

  glm::vec2 GetImageSize() const {
    if (this->m_Drawable) {
      return this->m_Drawable->GetSize();
    }
    return {0.0f, 0.0f};
  }

private:
  std::string GetImagePath(int id) const;
};

#endif // MAP_BLOCK_HPP
