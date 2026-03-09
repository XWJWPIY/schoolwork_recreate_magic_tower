#ifndef MONSTER_HPP
#define MONSTER_HPP

#include "Actor.hpp"
#include <string>

class Monster : public Actor {
public:
  Monster();
  ~Monster() override = default;

  int getRewardGold() const { return reward_gold; }
  void setRewardGold(int amount) { reward_gold = amount; }

  int getRewardExp() const { return reward_exp; }
  void setRewardExp(int amount) { reward_exp = amount; }

  std::string getSpecialAbility() const { return special_ability; }
  void setSpecialAbility(const std::string &ability) {
    special_ability = ability;
  }

  // Execute battle logic with player, returning false if player dies
  bool onBattle(class Player *player);

private:
  int reward_gold = 0;
  int reward_exp = 0;
  std::string special_ability = ""; // e.g., "POISON", "VAMPIRE", "MAGIC_ATTACK"
};

#endif // MONSTER_HPP
