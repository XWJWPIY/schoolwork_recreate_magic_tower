#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Actor.hpp"
#include "FloorMap.hpp"
#include <memory>

class Player : public Actor, public std::enable_shared_from_this<Player> {
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

  // Key management
  int GetYellowKeys() const { return m_yellow_keys; }
  int GetBlueKeys() const { return m_blue_keys; }
  int GetRedKeys() const { return m_red_keys; }

  bool UseKey(AppUtil::Effect type, int count = 1);

  // Coin management
  int GetCoins() const { return m_coins; }
  void AddCoins(int amount) { m_coins += amount; }
  void SetCoins(int count) { m_coins = count; }

  void ApplyEffect(AppUtil::Effect type, int value);

  void SetPendingShop(int id) { m_pending_shop_id = id; }
  int GetPendingShop() const { return m_pending_shop_id; }

  void ResetStateAfterFloorChange();
  void SetDirection(PlayerDirection dir) { m_direction = dir; }
  void SetIsAnimating(bool animate) { m_is_animating = animate; }
  void SetCurrentFrame(int frame) { m_current_frame = frame; }

public:
  void ObjectUpdate() override;

private:
  void UpdateSprite();

  PlayerDirection m_direction = PlayerDirection::DOWN;
  int m_current_frame = 1;
  bool m_is_animating = false;
  float m_animation_timer = 0.0f;
  const float FRAME_INTERVAL = 0.05f; // 50ms per frame for a fast movement feel

  int m_yellow_keys = 0;
  int m_blue_keys = 0;
  int m_red_keys = 0;
  int m_coins = 0;
  int m_pending_shop_id = -1;
};

#endif // PLAYER_HPP
