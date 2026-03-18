#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Entity.hpp"
#include "FloorMap.hpp"
#include <memory>

class Player : public Entity, public std::enable_shared_from_this<Player> {
public:
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

  void AddKey(int id);
  bool UseKey(int doorId);

  // Coin management
  int GetCoins() const { return m_coins; }
  void AddCoins(int amount) { m_coins += amount; }
  void SetCoins(int count) { m_coins = count; }

  void ApplyEffect(AppUtil::Effect type, int value);

private:
  int m_yellow_keys = 0;
  int m_blue_keys = 0;
  int m_red_keys = 0;
  int m_coins = 0;
};

#endif // PLAYER_HPP
