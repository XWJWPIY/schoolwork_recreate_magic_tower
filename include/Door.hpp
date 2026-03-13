#ifndef DOOR_HPP
#define DOOR_HPP

#include "Entity.hpp"
#include "Util/Logger.hpp"
#include "pch.hpp"

#include <string>

class Door : public Entity {
public:
  Door(int id);
  ~Door() override = default;

  void reaction() override;

  void ObjectUpdate() override;

private:
  static constexpr int MAX_ANIMATION_FRAMES = 5;
  bool m_IsOpening = false;
  int m_AnimationFrame = 1;
  float m_FrameTimer = 0.0f;
  const float FRAME_DELAY = 0.1f; // 100ms per frame
};

#endif // DOOR_HPP
