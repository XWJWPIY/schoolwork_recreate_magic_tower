#ifndef STAIR_HPP
#define STAIR_HPP

#include "Objects/Entity.hpp"
#include "Util/Logger.hpp"
#include "pch.hpp"

#include <functional>
#include <string>

class Stair : public Entity {
public:
  using TriggerCallback = std::function<void(int value, bool isRelative, int x, int y)>;

  Stair(int id, TriggerCallback callback);
  ~Stair() override = default;

  void Reaction(std::shared_ptr<Player> player) override;
  bool ShouldSkipWalkAnimation() const override { return true; }
  bool IsRelative() const { return m_isRelative; }

private:
  TriggerCallback m_on_trigger;
  bool m_isRelative;
};

#endif // STAIR_HPP
