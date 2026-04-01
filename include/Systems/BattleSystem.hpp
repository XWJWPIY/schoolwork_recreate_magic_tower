#ifndef BATTLESYSTEM_HPP
#define BATTLESYSTEM_HPP

#include <memory>
#include <vector>

class Player;
class Enemy;

class BattleSystem {
public:
    // Represents the final outcome and states of exactly one turn
    struct TurnResult {
        bool evading = false;
        bool instantKill = false;
        bool weakened = false;
        bool poisoned = false;
        int totalDamage = 0;
        bool isBattleEnd = false;
        int rewardExp = 0;
        int rewardCoin = 0;
    };

    /**
     * @brief Execute one attack turn from the Player against the Enemy.
     * @return TurnResult containing hit info and potential rewards if enemy died.
     */
    static TurnResult ProcessPlayerTurn(std::shared_ptr<Player> player, std::shared_ptr<Enemy> enemy);

    /**
     * @brief Execute one combat turn from the Enemy against the Player.
     * @return TurnResult containing multi-hit damage and status effect procs.
     */
    static TurnResult ProcessEnemyTurn(std::shared_ptr<Player> player, std::shared_ptr<Enemy> enemy);
};

#endif // BATTLESYSTEM_HPP
