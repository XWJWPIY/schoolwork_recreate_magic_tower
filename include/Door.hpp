#ifndef DOOR_HPP
#define DOOR_HPP

#include "Entity.hpp"
#include "Util/Logger.hpp"
#include "pch.hpp"

#include <string>

class Door : public Entity {
public:
  Door(int id);
  ~Door() override = default;

  void reaction() override;
};

#endif // DOOR_HPP
