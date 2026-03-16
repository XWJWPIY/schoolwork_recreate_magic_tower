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

  bool CanReact() const { return m_can_react; }
  void SetCanReact(bool value) { m_can_react = value; }

  void SetObjectId(int newId) override;
  void UpdateProperties(int id);

  virtual void Reaction(std::shared_ptr<Player> player) = 0;
  virtual void ObjectUpdate() override {}

  void SetReplacementComponent(std::shared_ptr<DynamicReplacementComponent> comp) {
    m_replacement_comp = comp;
  }

  void SetGridPosition(int x, int y) {
    m_grid_x = x;
    m_grid_y = y;
  }
  int GetGridX() const { return m_grid_x; }
  int GetGridY() const { return m_grid_y; }

  // Getters and Setters
  int GetHp() const { return m_hp; }
  void SetHp(int value) { m_hp = value; }

  int GetAttack() const { return m_attack; }
  void SetAttack(int value) { m_attack = value; }

  int GetDefense() const { return m_defense; }
  void SetDefense(int value) { m_defense = value; }

  int GetLevel() const { return m_level; }
  void SetLevel(int value) { m_level = value; }

  void SetMovable(bool value) { m_is_movable = value; }
  bool GetMovable() const { return m_is_movable; }

protected:
  int m_grid_x = 0;
  int m_grid_y = 0;

  bool m_can_react = true;
  bool m_is_movable = false;
  int m_hp = 0;
  int m_attack = 0;
  int m_defense = 0;
  int m_level = 1;

  int m_current_frame = 0;

  std::shared_ptr<DynamicReplacementComponent> m_replacement_comp;
};

#endif // ENTITY_HPP
