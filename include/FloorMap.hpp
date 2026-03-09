#ifndef FLOOR_MAP_HPP
#define FLOOR_MAP_HPP

#include "AppUtil.hpp"
#include "MapBlock.hpp"
#include "Util/GameObject.hpp"
#include <memory>
#include <vector>

class FloorMap : public Util::GameObject {
public:
  FloorMap(float centerX = 0.0f, float centerY = 0.0f, float scaleX = 1.0f,
           float scaleY = 1.0f);
  ~FloorMap() override = default;

  void
  LoadFloorData(const std::vector<std::vector<AppUtil::MapCell>> &floorData);

  void LoadFloorData(const std::vector<std::vector<int>> &floorData);

  std::shared_ptr<MapBlock> GetBlock(int x, int y);

  void SetAllBlocksVisible(bool visible) {
    for (auto &row : m_Blocks) {
      for (auto &block : row) {
        block->SetVisible(visible);
      }
    }
  }

private:
  std::vector<std::vector<std::shared_ptr<MapBlock>>> m_Blocks;
  const float BLOCK_SIZE = 48.0f; // 圖片路徑錯誤時使用
};

#endif // FLOOR_MAP_HPP
