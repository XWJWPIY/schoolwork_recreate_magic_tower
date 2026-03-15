#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "AllObjects.hpp"
#include "pch.hpp"
#include "DynamicReplacementComponent.hpp"
#include <memory>
#include <string>
#include <vector>

// #include "Util/Image.hpp" // Placeholder for PTSD Image

class Player;

class Entity : public AllObjects {
public:
  Entity(int initialId, const std::string &imagePath, bool canReact = true);
  ~Entity() override = default;

  bool CanReact() const { return m_CanReact; }
  void SetCanReact(bool value) { m_CanReact = value; }

  void SetObjectId(int newId) override;
  void UpdateProperties(int id);

  virtual void reaction(std::shared_ptr<Player> player) = 0;
  virtual void ObjectUpdate() override {}

  void SetReplacementComponent(std::shared_ptr<DynamicReplacementComponent> comp) {
    m_ReplacementComp = comp;
  }

  void SetGridPosition(int x, int y) {
    m_GridX = x;
    m_GridY = y;
  }
  int GetGridX() const { return m_GridX; }
  int GetGridY() const { return m_GridY; }

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
  int m_GridX = 0;
  int m_GridY = 0;

  bool m_CanReact = true;
  bool is_movable = false;
  int hp = 0;
  int attack = 0;
  int defense = 0;
  int level = 1;

  int current_frame = 0;

  std::shared_ptr<DynamicReplacementComponent> m_ReplacementComp;
};

#endif // ENTITY_HPP
