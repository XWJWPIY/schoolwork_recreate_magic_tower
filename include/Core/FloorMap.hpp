#ifndef FLOOR_MAP_HPP
#define FLOOR_MAP_HPP

#include "Objects/Entity.hpp"
#include "Core/AppUtil.hpp"
#include "Util/Renderer.hpp"
#include "pch.hpp" // For glm::vec2 included by pch
#include <functional>
#include <memory>
#include <vector>

class FloorMap {
public:
  using ObjectFactory = std::function<std::shared_ptr<Entity>(int id)>;

  FloorMap(ObjectFactory factory, float centerX = 0.0f, float centerY = 0.0f,
           float scaleX = 1.0f, float scaleY = 1.0f, float zIndex = -5.0f,
           const glm::vec2 &baseSize = {0.0f, 0.0f});
  ~FloorMap() = default;

  glm::vec2 GetBaseSize() const { return m_base_size; }

  void LoadFloorData(const std::vector<std::vector<int>> &floorData,
                     int story = -1);

  /**
   * @brief Load all floors from a prefix path (e.g. "Datas/Maps/RoadMap")
   */
  void LoadAllFloors(const std::string &prefix);

  std::shared_ptr<Entity> GetObject(int x, int y, int story = -1);
  bool IsPassable(int x, int y, int story = -1);

  void SwitchStory(int story);

  void Update();

  int GetCurrentStory() const { return m_current_story; }
  void SetObject(int x, int y, int id, int story = -1);

  void SetAllVisible(bool visible);
  glm::ivec2 FindFirstObjectPosition(int id, int story = -1);
  std::shared_ptr<Entity> FindFirstObjectOfId(int id, int story = -1);

  // Set the Root Renderer so FloorMap can Add/Remove objects dynamically
  void SetRenderer(Util::Renderer *root) { m_root = root; }
  void AddToRenderer();

private:
  Util::Renderer *m_root = nullptr;
  ObjectFactory m_factory;
  std::vector<std::vector<std::vector<std::shared_ptr<Entity>>>> m_objects;
  int m_current_story = 0;
  const float DEFAULT_SIZE =
      48.0f; // Default size used when image path is missing
  glm::vec2 m_base_size = {48.0f, 48.0f}; // stored dynamic base image size

  float m_center_x = 0.0f;
  float m_center_y = 0.0f;
  float m_scale_x = 1.0f;
  float m_scale_y = 1.0f;
  float m_z_index = -5.0f;

  void UpdateObjectAt(int x, int y, int id, int story);
  glm::vec2 GetGridAbsolutePosition(int x, int y) const;
};

#endif // FLOOR_MAP_HPP
