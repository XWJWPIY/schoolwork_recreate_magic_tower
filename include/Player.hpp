#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Actor.hpp"

class Player : public Actor {
public:
  Player();
  ~Player() override = default;

  int getGold() const { return gold; }
  void addGold(int amount) { gold += amount; }

  int getExp() const { return exp; }
  void addExp(int amount) { exp += amount; }

  int getYellowKey() const { return yellow_key; }
  void addYellowKey(int amount) { yellow_key += amount; }

  int getBlueKey() const { return blue_key; }
  void addBlueKey(int amount) { blue_key += amount; }

  int getRedKey() const { return red_key; }
  void addRedKey(int amount) { red_key += amount; }

private:
  int gold = 0;
  int exp = 0;

  int yellow_key = 0;
  int blue_key = 0;
  int red_key = 0;
};

#endif // PLAYER_HPP
