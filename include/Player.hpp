#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Actor.hpp"
#include "FloorMap.hpp"
#include <memory>

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

  void SetPendingShop(int id) { m_pending_shop_id = id; }
  int GetPendingShop() const { return m_pending_shop_id; }
  
  void ResetStateAfterFloorChange();
  void SetDirection(PlayerDirection dir);
  void SetIsAnimating(bool animate) { m_is_animating = animate; }

  void OnAttributeChanged(AppUtil::Effect type) override;

  void ObjectUpdate() override;

  PlayerDirection m_direction = PlayerDirection::DOWN;
  bool m_is_animating = false;
  std::shared_ptr<Util::Animation> m_animations[4]; // 0:Down, 1:Up, 2:Left, 3:Right

  int m_pending_shop_id = -1;
};


#endif // PLAYER_HPP
