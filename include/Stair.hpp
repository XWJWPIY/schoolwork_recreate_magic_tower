#ifndef STAIR_HPP
#define STAIR_HPP

#include "Entity.hpp"
#include "Util/Logger.hpp"
#include "pch.hpp"

#include <functional>
#include <string>

class Stair : public Entity {
public:
  using TriggerCallback = std::function<void(int delta)>;

  Stair(int id, TriggerCallback callback);
  ~Stair() override = default;

  bool IsPassable() const override;
  void reaction(std::shared_ptr<Player> player) override;

private:
  TriggerCallback m_OnTrigger;
};

#endif // STAIR_HPP
