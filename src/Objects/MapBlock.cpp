#include "Objects/MapBlock.hpp"
#include "Core/AppUtil.hpp"
#include <memory>
#include <string>

MapBlock::MapBlock(int initialId)
    // Initialize Entity with deep Z-Index.
    : Entity(initialId) {
  SetZIndex(-5);
}
