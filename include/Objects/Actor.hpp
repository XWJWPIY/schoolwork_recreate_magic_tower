#ifndef ACTOR_HPP
#define ACTOR_HPP

#include "Objects/Entity.hpp"
#include <unordered_map>
#include "Core/AppUtil.hpp"


class Actor : public Entity {
public:
  Actor(int initialId, bool canReact = true);
  ~Actor() override = default;

  // Unified Attribute Accessors
  virtual int GetAttr(AppUtil::Effect type) const;
  virtual void SetAttr(AppUtil::Effect type, int value);
  virtual void ApplyEffect(AppUtil::Effect type, int delta);

  bool MeetsRequirement(AppUtil::Effect type, int amount) const;

  virtual void OnAttributeChanged(AppUtil::Effect type);

protected:
  std::unordered_map<AppUtil::Effect, int> m_attributes;
};

#endif // ACTOR_HPP
