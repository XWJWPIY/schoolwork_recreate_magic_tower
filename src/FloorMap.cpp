#include "FloorMap.hpp"
#include "AllObjects.hpp"
#include "Entity.hpp"
#include "MapBlock.hpp"
#include "Util/Logger.hpp"

FloorMap::FloorMap(ObjectFactory factory, float centerX, float centerY,
                   float scaleX, float scaleY, float zIndex,
                   const glm::vec2 &baseSize)
    : m_Factory(factory), m_CenterX(centerX), m_CenterY(centerY),
      m_ScaleX(scaleX), m_ScaleY(scaleY), m_ZIndex(zIndex) {

  glm::vec2 currentBaseSize = baseSize;

  // If baseSize was not provided (is 0,0), try to sample from factory
  if (currentBaseSize.x <= 0 || currentBaseSize.y <= 0) {
    auto sampleObj = m_Factory(0);
    if (sampleObj) {
      currentBaseSize = sampleObj->GetScaledSize() / sampleObj->m_Transform.scale;
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

  m_BaseSize = currentBaseSize; 

  float spacingX = currentBaseSize.x * scaleX;
  float spacingY = currentBaseSize.y * scaleY;

  for (int s = 0; s < AppUtil::TOTAL_STORY; ++s) {
    std::vector<std::vector<std::shared_ptr<AllObjects>>> storyObjects;
    for (int y = 0; y < 11; ++y) {
      std::vector<std::shared_ptr<AllObjects>> row;
      for (int x = 0; x < 11; ++x) {
        auto obj = m_Factory(0);

        if (obj) {
          obj->m_Transform.scale = {scaleX, scaleY};
          obj->SetZIndex(m_ZIndex);
          float absX = centerX + (x - 5) * spacingX;
          float absY = centerY + (5 - y) * spacingY;

          obj->m_Transform.translation = {absX, absY};

          // Only the current story (0) is visible initially
          obj->SetVisible(s == m_CurrentStory && obj->GetObjectId() != 0);

          if (m_Root)
            m_Root->AddChild(obj);

          auto entity = std::dynamic_pointer_cast<Entity>(obj);
          if (entity) {
            entity->SetGridPosition(x, y);
          }
        }
        row.push_back(obj);
      }
      storyObjects.push_back(row);
    }
    m_Objects.push_back(storyObjects);
  }
}

void FloorMap::LoadFloorData(
    const std::vector<std::vector<AppUtil::MapCell>> &floorData, int story) {
  int targetStory = (story == -1) ? m_CurrentStory : story;
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
      auto oldObj = m_Objects[targetStory][y][x];

      if ((!oldObj && newId == 0) ||
          (oldObj && oldObj->GetObjectId() == newId)) {
        continue;
      }

      // Need to replace the object
      auto newObj = m_Factory(newId);

      // If we are placing a valid object
      if (newObj) {
        newObj->SetZIndex(m_ZIndex);
        // Inherit transform from the old object if it existed
        if (oldObj) {
          newObj->m_Transform = oldObj->m_Transform;
          if (m_Root)
            m_Root->RemoveChild(oldObj);
        } else {
          // Calculate transform based on grid position
          newObj->m_Transform.scale = {m_ScaleX, m_ScaleY};
          float spacingX = m_BaseSize.x * m_ScaleX;
          float spacingY = m_BaseSize.y * m_ScaleY;
          float absX = m_CenterX + (x - 5) * spacingX;
          float absY =
              m_CenterY +
              (5 - y) * spacingY; 
          newObj->m_Transform.translation = {absX, absY};
        }

        // Visibility logic
        newObj->SetVisible(targetStory == m_CurrentStory && newId != 0);

        if (m_Root)
          m_Root->AddChild(newObj);
      } else if (oldObj) {
        // If the new object is empty (id=0) but we had an object here, remove it
        if (m_Root)
          m_Root->RemoveChild(oldObj);
      }

      m_Objects[targetStory][y][x] = newObj;

      auto entity = std::dynamic_pointer_cast<Entity>(newObj);
      if (entity) {
        entity->SetGridPosition(x, y);
      }
    }
  }
}

void FloorMap::LoadFloorData(const std::vector<std::vector<int>> &floorData,
                             int story) {
  int targetStory = (story == -1) ? m_CurrentStory : story;
  if (targetStory < 0 || targetStory >= AppUtil::TOTAL_STORY)
    return;

  if (floorData.size() != 11)
    return;
  for (int y = 0; y < 11; ++y) {
    if (floorData[y].size() != 11)
      return;
    for (int x = 0; x < 11; ++x) {
      int newId = floorData[y][x];
      auto oldObj = m_Objects[targetStory][y][x];

      if ((!oldObj && newId == 0) ||
          (oldObj && oldObj->GetObjectId() == newId)) {
        continue;
      }

      // Need to replace the object
      auto newObj = m_Factory(newId);

      if (newObj) {
        newObj->SetZIndex(m_ZIndex);
        if (oldObj) {
          newObj->m_Transform = oldObj->m_Transform;
          if (m_Root)
            m_Root->RemoveChild(oldObj);
        } else {
          newObj->m_Transform.scale = {m_ScaleX, m_ScaleY};
          float spacingX = m_BaseSize.x * m_ScaleX;
          float spacingY = m_BaseSize.y * m_ScaleY;
          float absX = m_CenterX + (x - 5) * spacingX;
          float absY = m_CenterY + (5 - y) * spacingY;
          newObj->m_Transform.translation = {absX, absY};
        }

        // Visibility logic
        newObj->SetVisible(targetStory == m_CurrentStory && newId != 0);

        if (m_Root)
          m_Root->AddChild(newObj);
      } else if (oldObj) {
        if (m_Root)
          m_Root->RemoveChild(oldObj);
      }
      m_Objects[targetStory][y][x] = newObj;

      auto entity = std::dynamic_pointer_cast<Entity>(newObj);
      if (entity) {
        entity->SetGridPosition(x, y);
      }
    }
  }
}

void FloorMap::SetObject(int x, int y, int id, int story) {
  int targetStory = (story == -1) ? m_CurrentStory : story;
  if (targetStory < 0 || targetStory >= AppUtil::TOTAL_STORY)
    return;

  if (x < 0 || x >= 11 || y < 0 || y >= 11)
    return;

  auto oldObj = m_Objects[targetStory][y][x];
  if (oldObj && oldObj->GetObjectId() == id) {
    return;
  }

  auto newObj = m_Factory(id);
  if (newObj) {
    newObj->SetZIndex(m_ZIndex);
    if (oldObj) {
      newObj->m_Transform = oldObj->m_Transform;
      if (m_Root)
        m_Root->RemoveChild(oldObj);
    } else {
      newObj->m_Transform.scale = {m_ScaleX, m_ScaleY};
      float spacingX = m_BaseSize.x * m_ScaleX;
      float spacingY = m_BaseSize.y * m_ScaleY;
      float absX = m_CenterX + (x - 5) * spacingX;
      float absY = m_CenterY + (5 - y) * spacingY;
      newObj->m_Transform.translation = {absX, absY};
    }

    // Set grid position for entities
    auto entity = std::dynamic_pointer_cast<Entity>(newObj);
    if (entity) {
      entity->SetGridPosition(x, y);
    }

    // Visibility logic
    newObj->SetVisible(targetStory == m_CurrentStory && id != 0);

    if (m_Root)
      m_Root->AddChild(newObj);
  } else if (oldObj) {
    if (m_Root)
      m_Root->RemoveChild(oldObj);
  }
  m_Objects[targetStory][y][x] = newObj;
}

std::shared_ptr<AllObjects> FloorMap::GetObject(int x, int y, int story) {
  int targetStory = (story == -1) ? m_CurrentStory : story;
  if (targetStory < 0 || targetStory >= AppUtil::TOTAL_STORY)
    return nullptr;

  if (x >= 0 && x < 11 && y >= 0 && y < 11) {
    return m_Objects[targetStory][y][x];
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
  if (story < 0 || story >= AppUtil::TOTAL_STORY || story == m_CurrentStory)
    return;

  // Hide current story
  for (auto &row : m_Objects[m_CurrentStory]) {
    for (auto &obj : row) {
      if (obj)
        obj->SetVisible(false);
    }
  }

  m_CurrentStory = story;

  // Show new story
  for (auto &row : m_Objects[m_CurrentStory]) {
    for (auto &obj : row) {
      if (obj && obj->GetObjectId() != 0) {
        obj->SetVisible(true);
      }
    }
  }
}

void FloorMap::Update() {
  for (auto &row : m_Objects[m_CurrentStory]) {
    for (auto &obj : row) {
      if (obj && obj->GetVisible()) {
        obj->ObjectUpdate();
      }
    }
  }
}

void FloorMap::AddToRenderer() {
  if (!m_Root)
    return;
  for (int s = 0; s < AppUtil::TOTAL_STORY; ++s) {
    for (auto &row : m_Objects[s]) {
      for (auto &obj : row) {
        if (obj) {
          m_Root->AddChild(obj);
        }
      }
    }
  }
}
