#include "FloorMap.hpp"
#include "AllObjects.hpp"
#include "Entity.hpp"
#include "MapBlock.hpp"
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

  // Fallback if somehow still 0
  if (currentBaseSize.x <= 0 || currentBaseSize.y <= 0) {
    currentBaseSize = {DEFAULT_SIZE, DEFAULT_SIZE};
  }

  m_base_size = currentBaseSize; 

  float spacingX = currentBaseSize.x * scaleX;
  float spacingY = currentBaseSize.y * scaleY;

  for (int s = 0; s < AppUtil::TOTAL_STORY; ++s) {
    std::vector<std::vector<std::shared_ptr<AllObjects>>> story_objects;
    for (int y = 0; y < 11; ++y) {
      std::vector<std::shared_ptr<AllObjects>> row;
      for (int x = 0; x < 11; ++x) {
        auto obj = m_factory(0);

        if (obj) {
          obj->m_Transform.scale = {scaleX, scaleY};
          obj->SetZIndex(m_z_index);
          float absX = centerX + (x - 5) * spacingX;
          float absY = centerY + (5 - y) * spacingY;

          obj->m_Transform.translation = {absX, absY};

          // Only the current story (0) is visible initially
          obj->SetVisible(s == m_current_story && obj->GetObjectId() != 0);

          if (m_root)
            m_root->AddChild(obj);

          auto entity = std::dynamic_pointer_cast<Entity>(obj);
          if (entity) {
            entity->SetGridPosition(x, y);
          }
        }
        row.push_back(obj);
      }
      story_objects.push_back(row);
    }
    m_objects.push_back(story_objects);
  }
}

void FloorMap::LoadFloorData(
    const std::vector<std::vector<AppUtil::MapCell>> &floorData, int story) {
  int targetStory = (story == -1) ? m_current_story : story;
  if (targetStory < 0 || targetStory >= AppUtil::TOTAL_STORY)
    return;

  if (floorData.size() != 11) {
    LOG_ERROR("Floor data Y size must be 11, strictly matching grid size.");
    return;
  }

  for (int y = 0; y < 11; ++y) {
    if (floorData[y].size() != 11) {
      LOG_ERROR("Floor data X size must be 11, strictly matching grid size.");
      return;
    }
    for (int x = 0; x < 11; ++x) {
      int newId = floorData[y][x].id;
      auto old_obj = m_objects[targetStory][y][x];

      if ((!old_obj && newId == 0) ||
          (old_obj && old_obj->GetObjectId() == newId)) {
        continue;
      }

      // Need to replace the object
      auto new_obj = m_factory(newId);

      // If we are placing a valid object
      if (new_obj) {
        new_obj->SetZIndex(m_z_index);
        // Inherit transform from the old object if it existed
        if (old_obj) {
          new_obj->m_Transform = old_obj->m_Transform;
          if (m_root)
            m_root->RemoveChild(old_obj);
        } else {
          // Calculate transform based on grid position
          new_obj->m_Transform.scale = {m_scale_x, m_scale_y};
          float spacingX = m_base_size.x * m_scale_x;
          float spacingY = m_base_size.y * m_scale_y;
          float absX = m_center_x + (x - 5) * spacingX;
          float absY =
              m_center_y +
              (5 - y) * spacingY; 
          new_obj->m_Transform.translation = {absX, absY};
        }

        // Visibility logic
        new_obj->SetVisible(targetStory == m_current_story && newId != 0);

        if (m_root)
          m_root->AddChild(new_obj);
      } else if (old_obj) {
        // If the new object is empty (id=0) but we had an object here, remove it
        if (m_root)
          m_root->RemoveChild(old_obj);
      }

      m_objects[targetStory][y][x] = new_obj;

      auto entity = std::dynamic_pointer_cast<Entity>(new_obj);
      if (entity) {
        entity->SetGridPosition(x, y);
      }
    }
  }
}

void FloorMap::LoadFloorData(const std::vector<std::vector<int>> &floorData,
                             int story) {
  int targetStory = (story == -1) ? m_current_story : story;
  if (targetStory < 0 || targetStory >= AppUtil::TOTAL_STORY)
    return;

  if (floorData.size() != 11)
    return;
  for (int y = 0; y < 11; ++y) {
    if (floorData[y].size() != 11)
      return;
    for (int x = 0; x < 11; ++x) {
      int newId = floorData[y][x];
      auto old_obj = m_objects[targetStory][y][x];

      if ((!old_obj && newId == 0) ||
          (old_obj && old_obj->GetObjectId() == newId)) {
        continue;
      }

      // Need to replace the object
      auto new_obj = m_factory(newId);

      if (new_obj) {
        new_obj->SetZIndex(m_z_index);
        if (old_obj) {
          new_obj->m_Transform = old_obj->m_Transform;
          if (m_root)
            m_root->RemoveChild(old_obj);
        } else {
          new_obj->m_Transform.scale = {m_scale_x, m_scale_y};
          float spacingX = m_base_size.x * m_scale_x;
          float spacingY = m_base_size.y * m_scale_y;
          float absX = m_center_x + (x - 5) * spacingX;
          float absY = m_center_y + (5 - y) * spacingY;
          new_obj->m_Transform.translation = {absX, absY};
        }

        // Visibility logic
        new_obj->SetVisible(targetStory == m_current_story && newId != 0);

        if (m_root)
          m_root->AddChild(new_obj);
      } else if (old_obj) {
        if (m_root)
          m_root->RemoveChild(old_obj);
      }
      m_objects[targetStory][y][x] = new_obj;

      auto entity = std::dynamic_pointer_cast<Entity>(new_obj);
      if (entity) {
        entity->SetGridPosition(x, y);
      }
    }
  }
}

void FloorMap::SetObject(int x, int y, int id, int story) {
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
      float spacingX = m_base_size.x * m_scale_x;
      float spacingY = m_base_size.y * m_scale_y;
      float absX = m_center_x + (x - 5) * spacingX;
      float absY = m_center_y + (5 - y) * spacingY;
      new_obj->m_Transform.translation = {absX, absY};
    }

    // Set grid position for entities
    auto entity = std::dynamic_pointer_cast<Entity>(new_obj);
    if (entity) {
      entity->SetGridPosition(x, y);
    }

    // Visibility logic
    new_obj->SetVisible(targetStory == m_current_story && id != 0);

    if (m_root)
      m_root->AddChild(new_obj);
  } else if (old_obj) {
    if (m_root)
      m_root->RemoveChild(old_obj);
  }
  m_objects[targetStory][y][x] = new_obj;
}

std::shared_ptr<AllObjects> FloorMap::GetObject(int x, int y, int story) {
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
  if (!obj) {
    return true; // Empty space is passable by default
  }

  auto mapBlock = std::dynamic_pointer_cast<MapBlock>(obj);
  if (mapBlock) {
    return mapBlock->IsPassable();
  }

  auto entity = std::dynamic_pointer_cast<Entity>(obj);
  if (entity) {
    // Entities like doors or monsters might have different passability logic.
    // For now, if it's an entity, we assume it's NOT passable if it's visible.
    // In the future, we can add a reaction system.
    //return !entity->GetVisible();
  }

  return true;
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

std::shared_ptr<AllObjects> FloorMap::FindFirstObjectOfId(int id, int story) {
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
