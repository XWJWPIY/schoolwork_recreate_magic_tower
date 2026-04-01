#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Objects/Actor.hpp"
#include "Util/Logger.hpp"
#include "pch.hpp"

#include <string>
#include <functional>

class Player;

class Enemy : public Actor {
public:
  Enemy(int id);
  ~Enemy() override = default;

  void Reaction(std::shared_ptr<Player> player) override;
  void OnDefeated(std::shared_ptr<Player> player);
  void SetBattleCallback(std::function<void(std::shared_ptr<Enemy>)> cb) { m_start_battle_cb = cb; }
  void SetRewardCallback(std::function<void(const std::string&)> cb) { m_load_reward_layer_cb = cb; }

private:
  std::function<void(std::shared_ptr<Enemy>)> m_start_battle_cb;
  std::function<void(const std::string&)> m_load_reward_layer_cb;
};

#endif // ENEMY_HPP
