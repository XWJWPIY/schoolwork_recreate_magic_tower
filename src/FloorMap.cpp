#include "FloorMap.hpp"
#include "Entity.hpp"
#include "MapBlock.hpp"
#include "Util/Logger.hpp"

FloorMap::FloorMap(BlockFactory factory, float centerX, float centerY,
                   float scaleX, float scaleY, float zIndex,
                   const glm::vec2 &baseSize)
    : m_Factory(factory), m_CenterX(centerX), m_CenterY(centerY),
      m_ScaleX(scaleX), m_ScaleY(scaleY), m_ZIndex(zIndex) {

  glm::vec2 blockSize = baseSize;

  // If baseSize was not provided (is 0,0), try to sample from factory
  if (blockSize.x <= 0 || blockSize.y <= 0) {
    auto sampleBlock = m_Factory(0);
    if (sampleBlock) {
      blockSize = sampleBlock->GetScaledSize() / sampleBlock->m_Transform.scale;
    }
  }

  // Fallback if still 0
  if (blockSize.x <= 0 || blockSize.y <= 0) {
    blockSize = {BLOCK_SIZE, BLOCK_SIZE};
  }

  // Fallback if somehow still 0
  if (blockSize.x <= 0 || blockSize.y <= 0) {
    blockSize = {BLOCK_SIZE, BLOCK_SIZE};
  }

  m_BlockSize = blockSize; // Store for later use in LoadFloorData

  float spacingX = blockSize.x * scaleX;
  float spacingY = blockSize.y * scaleY;

  for (int s = 0; s < AppUtil::TOTAL_STORY; ++s) {
    std::vector<std::vector<std::shared_ptr<AllObjects>>> storyBlocks;
    for (int y = 0; y < 11; ++y) {
      std::vector<std::shared_ptr<AllObjects>> row;
      for (int x = 0; x < 11; ++x) {
        auto block = m_Factory(0);

        if (block) {
          block->m_Transform.scale = {scaleX, scaleY};
          block->SetZIndex(m_ZIndex);
          float absX = centerX + (x - 5) * spacingX;
          float absY = centerY + (5 - y) * spacingY;

          block->m_Transform.translation = {absX, absY};

          // Only the current story (0) is visible initially
          block->SetVisible(s == m_CurrentStory && block->GetObjectId() != 0);

          if (m_Root)
            m_Root->AddChild(block);
        }
        row.push_back(block);
      }
      storyBlocks.push_back(row);
    }
    m_Blocks.push_back(storyBlocks);
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
      auto oldBlock = m_Blocks[targetStory][y][x];

      if ((!oldBlock && newId == 0) ||
          (oldBlock && oldBlock->GetObjectId() == newId)) {
        continue;
      }

      // Need to replace the block
      auto newBlock = m_Factory(newId);

      // If we are placing a valid block
      if (newBlock) {
        newBlock->SetZIndex(m_ZIndex);
        // Inherit transform from the old block if it existed
        if (oldBlock) {
          newBlock->m_Transform = oldBlock->m_Transform;
          if (m_Root)
            m_Root->RemoveChild(oldBlock);
        } else {
          // Calculate transform based on grid position
          newBlock->m_Transform.scale = {m_ScaleX, m_ScaleY};
          float spacingX = m_BlockSize.x * m_ScaleX;
          float spacingY = m_BlockSize.y * m_ScaleY;
          float absX = m_CenterX + (x - 5) * spacingX;
          float absY =
              m_CenterY +
              (5 - y) * spacingY; // Note: Use instance member m_CenterY
          newBlock->m_Transform.translation = {absX, absY};
        }

        // Visibility logic
        newBlock->SetVisible(targetStory == m_CurrentStory && newId != 0);

        if (m_Root)
          m_Root->AddChild(newBlock);
      } else if (oldBlock) {
        // If the new block is empty (id=0) but we had a block here, remove it
        if (m_Root)
          m_Root->RemoveChild(oldBlock);
      }

      m_Blocks[targetStory][y][x] = newBlock;
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
      auto oldBlock = m_Blocks[targetStory][y][x];

      if ((!oldBlock && newId == 0) ||
          (oldBlock && oldBlock->GetObjectId() == newId)) {
        continue;
      }

      // Need to replace the block
      auto newBlock = m_Factory(newId);

      if (newBlock) {
        newBlock->SetZIndex(m_ZIndex);
        if (oldBlock) {
          newBlock->m_Transform = oldBlock->m_Transform;
          if (m_Root)
            m_Root->RemoveChild(oldBlock);
        } else {
          newBlock->m_Transform.scale = {m_ScaleX, m_ScaleY};
          float spacingX = m_BlockSize.x * m_ScaleX;
          float spacingY = m_BlockSize.y * m_ScaleY;
          float absX = m_CenterX + (x - 5) * spacingX;
          float absY = m_CenterY + (5 - y) * spacingY;
          newBlock->m_Transform.translation = {absX, absY};
        }

        // Visibility logic
        newBlock->SetVisible(targetStory == m_CurrentStory && newId != 0);

        if (m_Root)
          m_Root->AddChild(newBlock);
      } else if (oldBlock) {
        if (m_Root)
          m_Root->RemoveChild(oldBlock);
      }
      m_Blocks[targetStory][y][x] = newBlock;
    }
  }
}

std::shared_ptr<AllObjects> FloorMap::GetBlock(int x, int y, int story) {
  int targetStory = (story == -1) ? m_CurrentStory : story;
  if (targetStory < 0 || targetStory >= AppUtil::TOTAL_STORY)
    return nullptr;

  if (x >= 0 && x < 11 && y >= 0 && y < 11) {
    return m_Blocks[targetStory][y][x];
  }
  return nullptr;
}

bool FloorMap::IsPassable(int x, int y, int story) {
  auto block = GetBlock(x, y, story);
  if (!block) {
    return true; // Empty space is passable by default
  }

  auto mapBlock = std::dynamic_pointer_cast<MapBlock>(block);
  if (mapBlock) {
    return mapBlock->IsPassable();
  }

  auto entity = std::dynamic_pointer_cast<Entity>(block);
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
  for (auto &row : m_Blocks[m_CurrentStory]) {
    for (auto &block : row) {
      if (block)
        block->SetVisible(false);
    }
  }

  m_CurrentStory = story;

  // Show new story
  for (auto &row : m_Blocks[m_CurrentStory]) {
    for (auto &block : row) {
      if (block && block->GetObjectId() != 0) {
        block->SetVisible(true);
      }
    }
  }
}

void FloorMap::AddToRenderer() {
  if (!m_Root)
    return;
  for (int s = 0; s < AppUtil::TOTAL_STORY; ++s) {
    for (auto &row : m_Blocks[s]) {
      for (auto &block : row) {
        if (block) {
          m_Root->AddChild(block);
        }
      }
    }
  }
}
