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
           float scaleX = 1.0f, float scaleY = 1.0f, float zIndex = -5.0f,
           const glm::vec2 &baseSize = {0.0f, 0.0f});
  ~FloorMap() = default;

  glm::vec2 GetBaseBlockSize() const { return m_BlockSize; }

  void
  LoadFloorData(const std::vector<std::vector<AppUtil::MapCell>> &floorData,
                int story = -1);

  void LoadFloorData(const std::vector<std::vector<int>> &floorData,
                     int story = -1);

  std::shared_ptr<AllObjects> GetBlock(int x, int y, int story = -1);
  bool IsPassable(int x, int y, int story = -1);

  void SwitchStory(int story);

  void Update();

  int GetCurrentStory() const { return m_CurrentStory; }
  void SetBlock(int x, int y, int id, int story = -1);

  void SetAllVisible(bool visible) {
    for (int s = 0; s < AppUtil::TOTAL_STORY; ++s) {
      for (auto &row : m_Blocks[s]) {
        for (auto &block : row) {
          if (block) {
            if (visible && s == m_CurrentStory && block->GetObjectId() == 0) {
              block->SetVisible(false);
            } else if (visible && s == m_CurrentStory) {
              block->SetVisible(true);
            } else {
              block->SetVisible(false);
            }
          }
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
  std::vector<std::vector<std::vector<std::shared_ptr<AllObjects>>>> m_Blocks;
  int m_CurrentStory = 0;
  const float BLOCK_SIZE =
      48.0f; // Default size used when image path is missing
  glm::vec2 m_BlockSize = {48.0f, 48.0f}; // stored dynamic base image size

  float m_CenterX = 0.0f;
  float m_CenterY = 0.0f;
  float m_ScaleX = 1.0f;
  float m_ScaleY = 1.0f;
  float m_ZIndex = -5.0f;
};

#endif // FLOOR_MAP_HPP
