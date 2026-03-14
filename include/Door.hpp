#ifndef DOOR_HPP
#define DOOR_HPP
#include "Entity.hpp"
#include "Util/Animation.hpp"
#include <vector>

class Door : public Entity {
public:
  Door(int id);
  ~Door() override = default;

  void reaction(std::shared_ptr<Player> player) override;

  void ObjectUpdate() override;

private:
  static constexpr int MAX_ANIMATION_FRAMES = 5;
  std::shared_ptr<Util::Animation> m_Animation;
};

#endif // DOOR_HPP
