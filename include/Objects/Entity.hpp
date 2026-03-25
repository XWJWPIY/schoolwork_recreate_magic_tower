#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "Objects/AllObjects.hpp"
#include "pch.hpp"
#include "Systems/DynamicReplacementComponent.hpp"
#include <memory>
#include <string>
#include <vector>

// #include "Util/Image.hpp" // Placeholder for PTSD Image

class Player;

class Entity : public AllObjects, public std::enable_shared_from_this<Entity> {
public:
  Entity(int initialId, const std::string &imagePath, bool canReact = true);
  ~Entity() override = default;

  bool CanReact() const { return m_can_react; }
  void SetCanReact(bool value) { m_can_react = value; }

  virtual void Reaction(std::shared_ptr<Player> player);

  void SetReplacementComponent(std::shared_ptr<DynamicReplacementComponent> comp) {
    m_replacement_comp = comp;
  }
  void TriggerReplacement(int targetId = 0);

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

  std::shared_ptr<DynamicReplacementComponent> m_replacement_comp;
};

#endif // ENTITY_HPP
