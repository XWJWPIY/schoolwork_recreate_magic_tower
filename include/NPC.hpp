#ifndef NPC_HPP
#define NPC_HPP

#include "Entity.hpp"
#include "Util/Logger.hpp"
#include "pch.hpp"

#include <string>

class NPC : public Entity {
public:
  NPC(int id);
  ~NPC() override = default;

  void reaction() override;
};

#endif // NPC_HPP
