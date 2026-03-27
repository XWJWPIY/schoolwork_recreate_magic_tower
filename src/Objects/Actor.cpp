#include "Objects/Actor.hpp"
#include "Core/AppUtil.hpp"

Actor::Actor(int initialId, const std::string &imagePath, bool canReact)
    : Entity(initialId, imagePath, canReact) {
  
  // Load initial attributes from the registry using absolute assignment
  ForEachAttribute([this](AppUtil::Effect eff, int val) {
      SetAttr(eff, val);
  });
}

bool Actor::MeetsRequirement(AppUtil::Effect type, int amount) const {
    int current = GetAttr(type);
    if (type == AppUtil::Effect::HP) return current > amount;
    return current >= amount;
}
