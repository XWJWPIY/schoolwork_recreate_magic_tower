#include "Core/FloorMap.hpp"
#include "Objects/Entity.hpp"
#include "Objects/MapBlock.hpp"
#include "Util/Logger.hpp"

FloorMap::FloorMap(ObjectFactory factory, float centerX, float centerY,
                   float scaleX, float scaleY, float zIndex,
                   const glm::vec2 &baseSize)
    : m_factory(factory), m_center_x(centerX), m_center_y(centerY),
      m_scale_x(scaleX), m_scale_y(scaleY), m_z_index(zIndex) {

  glm::vec2 currentBaseSize = baseSize;

  // If baseSize was not provided (is 0,0), try to sample from factory
  if (currentBaseSize.x <= 0 || currentBaseSize.y <= 0) {
    auto sample_obj = m_factory(0);
    if (sample_obj) {
      currentBaseSize = sample_obj->GetScaledSize() / sample_obj->m_Transform.scale;
    }
  }

  // Fallback if still 0
  if (currentBaseSize.x <= 0 || currentBaseSize.y <= 0) {
    currentBaseSize = {DEFAULT_SIZE, DEFAULT_SIZE};
  }

  m_base_size = currentBaseSize; 

  float spacingX = currentBaseSize.x * scaleX;
  float spacingY = currentBaseSize.y * scaleY;

  for (int s = 0; s < AppUtil::TOTAL_STORY; ++s) {
    std::vector<std::vector<std::shared_ptr<Entity>>> story_objects;
    for (int y = 0; y < 11; ++y) {
      std::vector<std::shared_ptr<Entity>> row;
      for (int x = 0; x < 11; ++x) {
        row.push_back(nullptr); // Initialize with null, will be filled by UpdateObjectAt
      }
      story_objects.push_back(row);
    }
    m_objects.push_back(story_objects);
  }

  // Populate initial layer
  for (int s = 0; s < AppUtil::TOTAL_STORY; ++s) {
    for (int y = 0; y < 11; ++y) {
      for (int x = 0; x < 11; ++x) {
        UpdateObjectAt(x, y, 0, s);
      }
    }
  }
}

void FloorMap::LoadAllFloors(const std::string &prefix) {
  for (int i = 0; i < AppUtil::TOTAL_STORY; ++i) {
    std::string path = AppUtil::GetStaticResourcePath(prefix + std::to_string(i) + ".csv");
    auto data = AppUtil::MapParser::ParseCsv(path);
    if (!data.empty()) {
      LoadFloorData(data, i);
    }
  }
}

void FloorMap::LoadOverlay(const std::string &relativePath, int story) {
    std::string fullPath = AppUtil::GetStaticResourcePath(relativePath);
    auto data = AppUtil::MapParser::ParseCsv(fullPath);
    if (data.empty()) {
        LOG_ERROR("FloorMap: Failed to load overlay from {}", fullPath);
        return;
    }

    int targetStory = (story == -1) ? m_current_story : story;
    LOG_INFO("FloorMap: Applying overlay from {} to story {}", relativePath, targetStory);

    for (size_t y = 0; y < data.size() && y < 11; ++y) {
        for (size_t x = 0; x < data[y].size() && x < 11; ++x) {
            int id = data[y][x];
            if (id != 0) {
                UpdateObjectAt(static_cast<int>(x), static_cast<int>(y), id, targetStory);
            }
        }
    }
}

void FloorMap::LoadFloorData(const std::vector<std::vector<int>> &floorData,
                             int floorLevel) {
  if (floorLevel < 0 || floorLevel >= AppUtil::TOTAL_STORY) {
    LOG_ERROR("Floor level out of bounds: {}", floorLevel);
    return;
  }

  if (floorData.size() != 11) {
    LOG_ERROR("Floor data Y size must be 11, strictly matching grid size.");
    return;
  }

  for (int y = 0; y < 11; ++y) {
    if (floorData[y].size() != 11) {
      LOG_ERROR("Floor data X size must be 11 at row {}", y);
      return;
    }

    for (int x = 0; x < 11; ++x) {
      UpdateObjectAt(x, y, floorData[y][x], floorLevel);
    }
  }
}

void FloorMap::SetObject(int x, int y, int id, int story) {
  UpdateObjectAt(x, y, id, story);
}

void FloorMap::UpdateObjectAt(int x, int y, int id, int story) {
  int targetStory = (story == -1) ? m_current_story : story;
  if (targetStory < 0 || targetStory >= AppUtil::TOTAL_STORY)
    return;

  if (x < 0 || x >= 11 || y < 0 || y >= 11)
    return;

  auto old_obj = m_objects[targetStory][y][x];
  if (old_obj && old_obj->GetObjectId() == id) {
    return;
  }

  auto new_obj = m_factory(id);
  if (new_obj) {
    new_obj->SetZIndex(m_z_index);
    if (old_obj) {
      new_obj->m_Transform = old_obj->m_Transform;
      if (m_root)
        m_root->RemoveChild(old_obj);
    } else {
      new_obj->m_Transform.scale = {m_scale_x, m_scale_y};
      new_obj->m_Transform.translation = GetGridAbsolutePosition(x, y);
    }

    new_obj->SetGridPosition(x, y);
    new_obj->SetVisible(targetStory == m_current_story && id != 0);

    if (m_root)
      m_root->AddChild(new_obj);
  } else if (old_obj) {
    if (m_root)
      m_root->RemoveChild(old_obj);
  }
  m_objects[targetStory][y][x] = new_obj;
}

glm::vec2 FloorMap::GetGridAbsolutePosition(int x, int y) const {
  float spacingX = m_base_size.x * m_scale_x;
  float spacingY = m_base_size.y * m_scale_y;
  float absX = m_center_x + (x - 5) * spacingX;
  float absY = m_center_y + (5 - y) * spacingY;
  
  LOG_DEBUG("FloorMap: Calc Pos({}, {}) using scale({}), center({}), base_size({}) -> ({}, {})",
            x, y, m_scale_x, m_center_x, m_base_size.x, absX, absY);
            
  return {absX, absY};
}

std::shared_ptr<Entity> FloorMap::GetObject(int x, int y, int story) {
  int targetStory = (story == -1) ? m_current_story : story;
  if (targetStory < 0 || targetStory >= AppUtil::TOTAL_STORY)
    return nullptr;

  if (x >= 0 && x < 11 && y >= 0 && y < 11) {
    return m_objects[targetStory][y][x];
  }
  return nullptr;
}

bool FloorMap::IsPassable(int x, int y, int story) {
  auto obj = GetObject(x, y, story);
  if (!obj) return true; // Empty space is passable by default
  return obj->IsPassable();
}

void FloorMap::SwitchStory(int story) {
  if (story < 0 || story >= AppUtil::TOTAL_STORY || story == m_current_story)
    return;

  // Hide current story
  for (auto &row : m_objects[m_current_story]) {
    for (auto &obj : row) {
      if (obj)
        obj->SetVisible(false);
    }
  }

  m_current_story = story;

  // Show new story
  for (auto &row : m_objects[m_current_story]) {
    for (auto &obj : row) {
      if (obj && obj->GetObjectId() != 0) {
        obj->SetVisible(true);
      }
    }
  }
}

void FloorMap::Update() {
  for (auto &row : m_objects[m_current_story]) {
    for (auto &obj : row) {
      if (obj && obj->GetVisible()) {
        obj->ObjectUpdate();
      }
    }
  }
}

void FloorMap::SetAllVisible(bool visible) {
  for (int s = 0; s < AppUtil::TOTAL_STORY; ++s) {
    for (auto &row : m_objects[s]) {
      for (auto &obj : row) {
        if (obj) {
          if (visible && s == m_current_story && obj->GetObjectId() == 0) {
            obj->SetVisible(false);
          } else if (visible && s == m_current_story) {
            obj->SetVisible(true);
          } else {
            obj->SetVisible(false);
          }
        }
      }
    }
  }
}

glm::ivec2 FloorMap::FindFirstObjectPosition(int id, int story) {
  int targetStory = (story == -1) ? m_current_story : story;
  if (targetStory < 0 || targetStory >= AppUtil::TOTAL_STORY)
    return {-1, -1};

  for (int y = 0; y < 11; ++y) {
    for (int x = 0; x < 11; ++x) {
      auto obj = m_objects[targetStory][y][x];
      if (obj && obj->GetObjectId() == id) {
        return {x, y};
      }
    }
  }
  return {-1, -1};
}

std::shared_ptr<Entity> FloorMap::FindFirstObjectOfId(int id, int story) {
  int targetStory = (story == -1) ? m_current_story : story;
  if (targetStory < 0 || targetStory >= AppUtil::TOTAL_STORY)
    return nullptr;

  for (int y = 0; y < 11; ++y) {
    for (int x = 0; x < 11; ++x) {
      auto& obj = m_objects[targetStory][y][x];
      if (obj && obj->GetObjectId() == id)
        return obj;
    }
  }
  return nullptr;
}

void FloorMap::AddToRenderer() {
  if (!m_root)
    return;
  for (int s = 0; s < AppUtil::TOTAL_STORY; ++s) {
    for (auto &row : m_objects[s]) {
      for (auto &obj : row) {
        if (obj) {
          m_root->AddChild(obj);
        }
      }
    }
  }
}
