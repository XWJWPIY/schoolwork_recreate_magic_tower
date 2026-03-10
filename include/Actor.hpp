#ifndef ACTOR_HPP
#define ACTOR_HPP

#include "AllObjects.hpp"
#include "pch.hpp"
#include <memory>
#include <string>
#include <vector>

// #include "Util/Image.hpp" // Placeholder for PTSD Image

class Actor : public AllObjects {
public:
  Actor(int initialId = 0);
  ~Actor() override = default;

  void SetObjectId(int newId) override;
  void UpdateProperties(int id);
  std::string GetImagePath(int id) const;

  virtual bool canMove(int delta_x, int delta_y);

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

  bool is_movable = false;
  int hp = 0;
  int attack = 0;
  int defense = 0;
  int level = 1;

  int current_frame = 0;
  // std::vector<std::shared_ptr<Util::Image>> animation_frames;
};

#endif // ACTOR_HPP
