#ifndef TRIGGER_HPP
#define TRIGGER_HPP

#include "Entity.hpp"
#include "pch.hpp"
#include <string>
#include <functional>

class Trigger : public Entity {
public:
  typedef std::function<void(std::shared_ptr<Trigger>, const std::string&)> TriggerCallback;

  Trigger(int id, TriggerCallback callback);
  ~Trigger() override = default;

  void Reaction(std::shared_ptr<Player> player) override;

private:
  TriggerCallback m_trigger_callback;
};

#endif // TRIGGER_HPP
