#ifndef FLOOR_MAP_HPP
#define FLOOR_MAP_HPP

#include "AllObjects.hpp"
#include "AppUtil.hpp"
#include "Util/Renderer.hpp"
#include "pch.hpp" // For glm::vec2 included by pch
#include <functional>
#include <memory>
#include <vector>

class FloorMap {
public:
  using BlockFactory = std::function<std::shared_ptr<AllObjects>(int id)>;

  FloorMap(BlockFactory factory, float centerX = 0.0f, float centerY = 0.0f,
           float scaleX = 1.0f, float scaleY = 1.0f, float zIndex = -5.0f);
  ~FloorMap() = default;

  void
  LoadFloorData(const std::vector<std::vector<AppUtil::MapCell>> &floorData);

  void LoadFloorData(const std::vector<std::vector<int>> &floorData);

  std::shared_ptr<AllObjects> GetBlock(int x, int y);

  void SetAllVisible(bool visible) {
    for (auto &row : m_Blocks) {
      for (auto &block : row) {
        if (block) {
          block->SetVisible(visible);
        }
      }
    }
  }

  // Set the Root Renderer so FloorMap can Add/Remove blocks dynamically
  void SetRenderer(Util::Renderer *root) { m_Root = root; }
  void AddToRenderer();

private:
  Util::Renderer *m_Root = nullptr;
  BlockFactory m_Factory;
  std::vector<std::vector<std::shared_ptr<AllObjects>>> m_Blocks;
  const float BLOCK_SIZE = 48.0f;         // 圖片路徑錯誤時使用
  glm::vec2 m_BlockSize = {48.0f, 48.0f}; // stored dynamic base image size

  float m_CenterX = 0.0f;
  float m_CenterY = 0.0f;
  float m_ScaleX = 1.0f;
  float m_ScaleY = 1.0f;
  float m_ZIndex = -5.0f;
};

#endif // FLOOR_MAP_HPP
