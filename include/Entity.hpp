#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "AllObjects.hpp"
#include "pch.hpp"
#include <memory>
#include <string>
#include <vector>

// #include "Util/Image.hpp" // Placeholder for PTSD Image

class Entity : public AllObjects {
public:
  Entity(int initialId, const std::string &imagePath, bool canReact = true);
  ~Entity() override = default;

  bool GetVisible() const { return m_Visible; }
  bool CanReact() const { return m_CanReact; }
  void SetCanReact(bool value) { m_CanReact = value; }

  void SetObjectId(int newId) override;
  void UpdateProperties(int id);

  virtual void reaction() = 0;
  virtual bool IsPassable() const { return false; }

  // Getters and Setters
  int getHp() const { return hp; }
  void setHp(int value) { hp = value; }

  int getAttack() const { return attack; }
  void setAttack(int value) { attack = value; }

  int getDefense() const { return defense; }
  void setDefense(int value) { defense = value; }

  int getLevel() const { return level; }
  void setLevel(int value) { level = value; }

  void setMovable(bool value) { is_movable = value; }
  bool getMovable() const { return is_movable; }

protected:
  int x;
  int y;

  bool m_CanReact = true;
  bool is_movable = false;
  int hp = 0;
  int attack = 0;
  int defense = 0;
  int level = 1;

  int current_frame = 0;
};

#endif // ENTITY_HPP
