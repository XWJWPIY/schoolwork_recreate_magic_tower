#ifndef ACTORPART_HPP
#define ACTORPART_HPP

#include "Objects/Entity.hpp"
#include <memory>

class Player;

class ActorPart : public Entity {
public:
  ActorPart(int id);
  ~ActorPart() override = default;

  void Reaction(std::shared_ptr<Player> player) override;
  bool CheckCondition(std::shared_ptr<Player> player) const override;
};

#endif // ACTORPART_HPP
