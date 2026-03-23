#include "Actor.hpp"
#include "AppUtil.hpp"

Actor::Actor(int initialId, const std::string &imagePath, bool canReact)
    : Entity(initialId, imagePath, canReact) {
  
  // Load initial attributes from the registry
  auto it = AppUtil::GlobalObjectRegistry.find(initialId);
  if (it != AppUtil::GlobalObjectRegistry.end()) {
      for (const auto& [attrId, value] : it->second.initial_attributes) {
          SetAttr(AppUtil::AttributeRegistry::ToEffect(attrId), value);
      }
  }
}
