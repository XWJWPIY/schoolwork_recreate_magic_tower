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
  using ObjectFactory = std::function<std::shared_ptr<AllObjects>(int id)>;

  FloorMap(ObjectFactory factory, float centerX = 0.0f, float centerY = 0.0f,
           float scaleX = 1.0f, float scaleY = 1.0f, float zIndex = -5.0f,
           const glm::vec2 &baseSize = {0.0f, 0.0f});
  ~FloorMap() = default;

  glm::vec2 GetBaseSize() const { return m_BaseSize; }

  void
  LoadFloorData(const std::vector<std::vector<AppUtil::MapCell>> &floorData,
                int story = -1);

  void LoadFloorData(const std::vector<std::vector<int>> &floorData,
                     int story = -1);

  std::shared_ptr<AllObjects> GetObject(int x, int y, int story = -1);
  bool IsPassable(int x, int y, int story = -1);

  void SwitchStory(int story);

  void Update();

  int GetCurrentStory() const { return m_CurrentStory; }
  void SetObject(int x, int y, int id, int story = -1);

  void SetAllVisible(bool visible) {
    for (int s = 0; s < AppUtil::TOTAL_STORY; ++s) {
      for (auto &row : m_Objects[s]) {
        for (auto &obj : row) {
          if (obj) {
            if (visible && s == m_CurrentStory && obj->GetObjectId() == 0) {
              obj->SetVisible(false);
            } else if (visible && s == m_CurrentStory) {
              obj->SetVisible(true);
            } else {
              obj->SetVisible(false);
            }
          }
        }
      }
    }
  }

  // Set the Root Renderer so FloorMap can Add/Remove objects dynamically
  void SetRenderer(Util::Renderer *root) { m_Root = root; }
  void AddToRenderer();

private:
  Util::Renderer *m_Root = nullptr;
  ObjectFactory m_Factory;
  std::vector<std::vector<std::vector<std::shared_ptr<AllObjects>>>> m_Objects;
  int m_CurrentStory = 0;
  const float DEFAULT_SIZE =
      48.0f; // Default size used when image path is missing
  glm::vec2 m_BaseSize = {48.0f, 48.0f}; // stored dynamic base image size

  float m_CenterX = 0.0f;
  float m_CenterY = 0.0f;
  float m_ScaleX = 1.0f;
  float m_ScaleY = 1.0f;
  float m_ZIndex = -5.0f;
};

#endif // FLOOR_MAP_HPP
