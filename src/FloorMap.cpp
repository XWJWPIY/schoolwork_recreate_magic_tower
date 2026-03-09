#include "FloorMap.hpp"
#include "Util/Logger.hpp"

FloorMap::FloorMap(float centerX, float centerY, float scaleX, float scaleY) {

  // First, create a temporary block to query its actual image pixel size.
  auto sampleBlock = std::make_shared<MapBlock>(0);
  glm::vec2 blockSize = sampleBlock->GetImageSize();
  // Fallback if image somehow has zero size
  if (blockSize.x <= 0 || blockSize.y <= 0) {
    blockSize = {BLOCK_SIZE, BLOCK_SIZE};
  }

  // Scaled spacing: image size * scale factor
  float spacingX = blockSize.x * scaleX;
  float spacingY = blockSize.y * scaleY;

  // Generate the 11x11 array, using absolute coordinates (center + scaled
  // offset).
  for (int y = 0; y < 11; ++y) {
    std::vector<std::shared_ptr<MapBlock>> row;
    for (int x = 0; x < 11; ++x) {
      auto block = std::make_shared<MapBlock>(0);

      // Apply scale to each block
      block->m_Transform.scale = {scaleX, scaleY};

      // (5,5) is the center cell -> offset (0,0) relative to center
      float absX = centerX + (x - 5) * spacingX;
      float absY = centerY + (5 - y) * spacingY;

      block->m_Transform.translation = {absX, absY};

      this->AddChild(block);
      row.push_back(block);
    }
    m_Blocks.push_back(row);
  }
}

void FloorMap::LoadFloorData(
    const std::vector<std::vector<AppUtil::MapCell>> &floorData) {
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
      m_Blocks[y][x]->SetBlockId(floorData[y][x].id);
    }
  }
}

void FloorMap::LoadFloorData(const std::vector<std::vector<int>> &floorData) {
  if (floorData.size() != 11)
    return;
  for (int y = 0; y < 11; ++y) {
    if (floorData[y].size() != 11)
      return;
    for (int x = 0; x < 11; ++x) {
      m_Blocks[y][x]->SetBlockId(floorData[y][x]);
    }
  }
}

std::shared_ptr<MapBlock> FloorMap::GetBlock(int x, int y) {
  if (x >= 0 && x < 11 && y >= 0 && y < 11) {
    return m_Blocks[y][x];
  }
  return nullptr;
}
