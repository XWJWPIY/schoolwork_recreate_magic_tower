#ifndef MAP_BLOCK_HPP
#define MAP_BLOCK_HPP

#include "Util/GameObject.hpp"
#include <string>

class MapBlock : public Util::GameObject {
public:
  MapBlock(int initialId = 0);
  ~MapBlock() override = default;

  void SetBlockId(int newId);

  int GetBlockId() const { return m_CurrentId; }

  glm::vec2 GetImageSize() const {
    if (m_Drawable) {
      return m_Drawable->GetSize();
    }
    return {0.0f, 0.0f};
  }

private:
  int m_CurrentId = -1; // setup
  std::string GetImagePath(int id) const;
};

#endif // MAP_BLOCK_HPP
