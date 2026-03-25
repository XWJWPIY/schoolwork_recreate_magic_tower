#ifndef DOOR_HPP
#define DOOR_HPP
#include "Entity.hpp"

class Door : public Entity {
public:
  Door(int id);
  ~Door() override = default;

  void Reaction(std::shared_ptr<Player> player) override;

  void ObjectUpdate() override;
};

#endif // DOOR_HPP
