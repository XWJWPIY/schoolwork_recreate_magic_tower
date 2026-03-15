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

  void reaction(std::shared_ptr<Player> player) override;

  // Key management
  int GetYellowKeys() const { return m_YellowKeys; }
  int GetBlueKeys() const { return m_BlueKeys; }
  int GetRedKeys() const { return m_RedKeys; }

  void AddKey(int id);
  bool UseKey(int doorId);

private:
  int m_YellowKeys = 0;
  int m_BlueKeys = 0;
  int m_RedKeys = 0;
};

#endif // PLAYER_HPP
