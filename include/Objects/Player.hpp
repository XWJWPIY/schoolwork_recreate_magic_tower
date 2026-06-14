#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Objects/Actor.hpp"
#include <memory>

class FloorMap;

class Player : public Actor {
public:
  enum class PlayerDirection {
    DOWN = 1,
    UP = 2,
    LEFT = 3,
    RIGHT = 4
  };

  Player();

  // Movement using grid coordinates
  void Move(int dx, int dy, std::shared_ptr<FloorMap> roadmap,
            std::shared_ptr<FloorMap> thingsmap);

  // Sync screen position by borrowing from FloorMap
  void SyncPosition(std::shared_ptr<FloorMap> roadmap);

  void Reaction(std::shared_ptr<Player> player) override;

  void ResetStateAfterFloorChange();
  void SetDirection(PlayerDirection dir);
  void SetIsAnimating(bool animate) { m_is_animating = animate; }
  bool HasFly() const { return GetAttr(AppUtil::Effect::FLY) > 0; }
  bool IsSuperMode() const { return m_is_super_mode; }
  bool GetIsWeak() const { return m_is_weak; }
  void SetIsWeak(bool weak) { m_is_weak = weak; }
  bool GetIsPoison() const { return m_is_poisoned; }
  void SetIsPoison(bool poisoned) { m_is_poisoned = poisoned; }


  void ObjectUpdate() override;

  void ToggleSuperMode();

  // Parallel Attribute Overrides
  int GetAttr(AppUtil::Effect type) const override;
  void SetAttr(AppUtil::Effect type, int value) override;
  void ApplyEffect(AppUtil::Effect type, int delta) override;

private:
  PlayerDirection m_direction = PlayerDirection::DOWN;
  bool m_is_animating = false;
  std::shared_ptr<Util::Animation> m_animations[4]; // 0:Down, 1:Up, 2:Left, 3:Right
  bool m_is_super_mode = false;
  bool m_is_weak = false;
  bool m_is_poisoned = false;
  std::unordered_map<AppUtil::Effect, int> m_super_attributes;
  std::shared_ptr<Util::Image> m_giraffe_image; // Giraffe skin for Super Mode
};


#endif // PLAYER_HPP
