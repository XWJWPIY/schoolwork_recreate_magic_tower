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
  void ObjectUpdate() override;

  void SetReplacementComponent(std::shared_ptr<DynamicReplacementComponent> comp) {
    m_replacement_comp = comp;
  }

  void SetGridPosition(int x, int y) {
    m_grid_x = x;
    m_grid_y = y;
  }
  int GetGridX() const { return m_grid_x; }
  int GetGridY() const { return m_grid_y; }

  void SetMovable(bool value) { m_is_movable = value; }
  bool GetMovable() const { return m_is_movable; }

protected:
  int m_grid_x = 0;
  int m_grid_y = 0;

  bool m_can_react = true;
  bool m_is_movable = false;

  int m_current_frame = 0;

  std::shared_ptr<DynamicReplacementComponent> m_replacement_comp;
};

#endif // ENTITY_HPP
