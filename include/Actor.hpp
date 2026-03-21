#ifndef ACTOR_HPP
#define ACTOR_HPP

#include "Entity.hpp"

class Actor : public Entity {
public:
  Actor(int initialId, const std::string &imagePath, bool canReact = true);
  ~Actor() override = default;

  // Getters and Setters
  int GetHp() const { return m_hp; }
  void SetHp(int value) { m_hp = value; }

  int GetAttack() const { return m_attack; }
  void SetAttack(int value) { m_attack = value; }

  int GetDefense() const { return m_defense; }
  void SetDefense(int value) { m_defense = value; }

  int GetLevel() const { return m_level; }
  void SetLevel(int value) { m_level = value; }

  int GetAgility() const { return m_agility; }
  void SetAgility(int value) { m_agility = value; }

  int GetExp() const { return m_exp; }
  void SetExp(int value) { m_exp = value; }

protected:
  int m_hp = 0;
  int m_attack = 0;
  int m_defense = 0;
  int m_level = 1;
  int m_agility = 0;
  int m_exp = 0;
};

#endif // ACTOR_HPP
