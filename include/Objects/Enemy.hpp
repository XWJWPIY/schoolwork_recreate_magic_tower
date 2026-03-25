#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Objects/Actor.hpp"
#include "Util/Logger.hpp"
#include "pch.hpp"

#include <string>

class Player;

class Enemy : public Actor {
public:
  Enemy(int id);
  ~Enemy() override = default;

  void Reaction(std::shared_ptr<Player> player) override;
};

#endif // ENEMY_HPP
