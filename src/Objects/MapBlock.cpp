#include "Objects/MapBlock.hpp"
#include "Core/AppUtil.hpp"
#include "Util/Image.hpp"
#include <memory>
#include <string>

MapBlock::MapBlock(int initialId)
    // Initialize AllObjects with deep Z-Index.
    : AllObjects(nullptr, -5, initialId) {
  SetObjectId(initialId);
}

std::string MapBlock::GetImagePath(int id) const {
    return AppUtil::GetFullResourcePath(id);
}
