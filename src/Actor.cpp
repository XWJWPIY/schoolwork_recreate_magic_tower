#include "Actor.hpp"
#include "AppUtil.hpp"

Actor::Actor(int initialId, const std::string &imagePath, bool canReact)
    : Entity(initialId, imagePath, canReact) {
  
  // Load initial attributes from the registry
  auto it = AppUtil::GlobalObjectRegistry.find(initialId);
  if (it != AppUtil::GlobalObjectRegistry.end()) {
      const auto& meta = it->second;
      for (const auto& [attrId, valStr] : meta.attributes) {
          if (valStr.empty()) continue;
          AppUtil::Effect eff = AppUtil::AttributeRegistry::ToEffect(attrId);
          if (eff != AppUtil::Effect::NONE) {
              try {
                  int val = std::stoi(valStr);
                  if (val != 0) SetAttr(eff, val);
              } catch (...) {}
          }
      }
  }
}
