#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"
#include "pch.hpp"
#include "Systems/DynamicReplacementComponent.hpp"
#include <memory>
#include <string>
#include <vector>

class Player;

class Entity : public Util::GameObject, public std::enable_shared_from_this<Entity> {
public:
  // Using protected constructor so it can't be instantiated accidentally as a base
  Entity(int initialId = 0);
  Entity(int initialId, const std::string &imagePath, bool canReact = true);
  
  // Forward constructor to GameObject to handle Drawable and ZIndex
  Entity(const std::shared_ptr<Core::Drawable> &drawable,
             const float zIndex, int initialId = 0);

  virtual ~Entity() override = default;

  // --- Core Object Identification & Rendering ---
  virtual void SetObjectId(int newId);
  int GetObjectId() const { return m_object_id; }

  virtual void ObjectUpdate();
  bool GetVisible() const { return this->m_Visible; }

  // Unified passability
  virtual bool IsPassable() const { return m_is_passable; }
  void SetPassable(bool passable) { m_is_passable = passable; }

  // --- Interaction & Logic ---
  bool CanReact() const { return m_can_react; }
  void SetCanReact(bool value) { m_can_react = value; }

  virtual void Reaction(std::shared_ptr<Player> player);
  
  // Decoupling behaviors
  virtual bool ShouldSkipWalkAnimation() const { return false; }

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
  // --- Animation Infrastructure ---
  void SetupAnimation(int id, bool looping, int intervalMs = 500);

  std::shared_ptr<Util::Animation> m_animation;
  std::string m_base_image_path;

  int m_object_id = 0;
  bool m_is_passable = true;

  int m_grid_x = 0;
  int m_grid_y = 0;

  bool m_can_react = true;
  bool m_is_movable = false;

  std::shared_ptr<DynamicReplacementComponent> m_replacement_comp;
};

#endif // ENTITY_HPP
