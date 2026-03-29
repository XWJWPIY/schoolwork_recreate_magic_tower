#ifndef ACTOR_HPP
#define ACTOR_HPP

#include "Objects/Entity.hpp"
#include <unordered_map>
#include "Core/AppUtil.hpp"

class Player;

class Actor : public Entity {
public:
  Actor(int initialId, bool canReact = true);
  ~Actor() override = default;

  // Unified Attribute Accessors
  int GetAttr(AppUtil::Effect type) const {
    auto it = m_attributes.find(type);
    return (it != m_attributes.end()) ? it->second : 0;
  }

  void SetAttr(AppUtil::Effect type, int value) {
    m_attributes[type] = value;
    OnAttributeChanged(type);
  }

  void ApplyEffect(AppUtil::Effect type, int delta) {
    m_attributes[type] += delta;
    OnAttributeChanged(type);
  }

  bool MeetsRequirement(AppUtil::Effect type, int amount) const;

  virtual void OnAttributeChanged(AppUtil::Effect type) {}

protected:
  std::unordered_map<AppUtil::Effect, int> m_attributes;
};

#endif // ACTOR_HPP
