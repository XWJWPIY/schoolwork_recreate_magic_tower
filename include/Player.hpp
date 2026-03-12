#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Entity.hpp"
#include "FloorMap.hpp"
#include <memory>

class Player : public Entity {
public:
  Player();

  // Movement using grid coordinates
  void Move(int dx, int dy, std::shared_ptr<FloorMap> roadmap);

  // Sync screen position by borrowing from FloorMap
  void SyncPosition(std::shared_ptr<FloorMap> roadmap);

  // Getters for grid position
  int GetGridX() const { return m_GridX; }
  int GetGridY() const { return m_GridY; }

  void SetGridPosition(int x, int y) {
    m_GridX = x;
    m_GridY = y;
  }

private:
  int m_GridX = 0;
  int m_GridY = 0;
};

#endif // PLAYER_HPP
