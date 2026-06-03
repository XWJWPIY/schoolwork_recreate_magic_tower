#include "Objects/Actor.hpp"
#include "Core/AppUtil.hpp"

Actor::Actor(int initialId, bool canReact)
    : Entity(initialId, canReact) {
  
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

int Actor::GetAttr(AppUtil::Effect type) const {
    auto it = m_attributes.find(type);
    return (it != m_attributes.end()) ? it->second : 0;
}

void Actor::SetAttr(AppUtil::Effect type, int value) {
    m_attributes[type] = value;
    if (type == AppUtil::Effect::HP && m_attributes[type] < 0) {
        m_attributes[type] = 0;
    }
    OnAttributeChanged(type);
}

void Actor::ApplyEffect(AppUtil::Effect type, int delta) {
    m_attributes[type] += delta;
    if (type == AppUtil::Effect::HP && m_attributes[type] < 0) {
        m_attributes[type] = 0;
    }
    OnAttributeChanged(type);
}

void Actor::OnAttributeChanged(AppUtil::Effect type) {
    // Base implementation does nothing
}
