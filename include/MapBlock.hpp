#ifndef MAP_BLOCK_HPP
#define MAP_BLOCK_HPP

#include "AllObjects.hpp"
#include "Util/Image.hpp"
#include <vector>

class MapBlock : public AllObjects {
public:
  MapBlock(int initialId = 0);
  ~MapBlock() override = default;

  void SetObjectId(int newId) override;
  void ObjectUpdate() override;

  glm::vec2 GetImageSize() const {
    if (this->m_Drawable) {
      return this->m_Drawable->GetSize();
    }
    return {0.0f, 0.0f};
  }


private:
  int m_CurrentLocalFrame = 1;
  std::string GetImagePath(int id) const;
  void UpdateProperties(int id);
};

#endif // MAP_BLOCK_HPP
