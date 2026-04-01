#include "Systems/BattleSystem.hpp"
#include "Objects/Player.hpp"
#include "Objects/Enemy.hpp"
#include "Core/AppUtil.hpp"
#include "Util/Logger.hpp"
#include <algorithm>

BattleSystem::TurnResult BattleSystem::ProcessPlayerTurn(std::shared_ptr<Player> player, std::shared_ptr<Enemy> enemy) {
    TurnResult result;
    if (!player || !enemy) return result;

    int eAGI = enemy->GetAttr(AppUtil::Effect::AGILITY);
    int roll = AppUtil::GetRandomInt(0, 99);

    if (roll < eAGI) {
        // Enemy evaded
        result.evading = true;
        result.totalDamage = 0;
        LOG_INFO("Enemy Evaded! (Roll: {} < AGI: {})", roll, eAGI);
    } else {
        // Player hits
        int eDef = enemy->GetAttr(AppUtil::Effect::DEFENSE);
        int pAtk = player->GetAttr(AppUtil::Effect::ATTACK);
        int dmg = std::max(1, pAtk - eDef); // Damage floor 1

        enemy->ApplyEffect(AppUtil::Effect::HP, -dmg);
        result.totalDamage = dmg;
        LOG_INFO("Hero Hits! (Roll: {} >= AGI: {})", roll, eAGI);
    }

    if (enemy->GetAttr(AppUtil::Effect::HP) <= 0) {
        result.isBattleEnd = true;
        auto meta = AppUtil::GlobalObjectRegistry[enemy->GetObjectId()];
        result.rewardExp = meta.GetInt(AppUtil::Attr::EXP);
        result.rewardCoin = meta.GetInt(AppUtil::Attr::COIN);
    }

    return result;
}

BattleSystem::TurnResult BattleSystem::ProcessEnemyTurn(std::shared_ptr<Player> player, std::shared_ptr<Enemy> enemy) {
    TurnResult result;
    if (!player || !enemy) return result;

    auto meta = AppUtil::GlobalObjectRegistry[enemy->GetObjectId()];
    bool ignoreDef = meta.GetInt("Ignore_DEF") > 0;
    int atkTime = meta.GetInt("ATK_Time", 1);
    int pAGI = player->GetAttr(AppUtil::Effect::AGILITY);

    std::string specialStr = meta.GetString("Special");
    bool isKilling = meta.GetInt("Killing_ATK") > 0 || specialStr == AppUtil::GetGlobalString("battle_special_kill", "Critical");
    bool isWeak = specialStr == AppUtil::GetGlobalString("battle_special_weak", "Weak");
    bool isPoison = specialStr == AppUtil::GetGlobalString("battle_special_poison", "Poison");

    int pDef = player->GetAttr(AppUtil::Effect::DEFENSE);
    int eAtk = enemy->GetAttr(AppUtil::Effect::ATTACK);
    int eDmgBase = ignoreDef ? eAtk : (eAtk - pDef);
    if (eDmgBase < 1) eDmgBase = 1; // Damage floor 1

    LOG_INFO("Enemy (ID: {}) Turn Start", enemy->GetObjectId());
    LOG_INFO("Enemy Stats: ATK_Time={}, IgnoreDef={}", atkTime, (ignoreDef ? 1 : 0));

    for (int i = 0; i < atkTime; ++i) {
        int killRoll = AppUtil::GetRandomInt(0, 99);
        if (isKilling && killRoll < 10) {
            result.totalDamage = player->GetAttr(AppUtil::Effect::HP);
            result.instantKill = true;
            LOG_INFO("Instant Kill! (Roll: {} < 10)", killRoll);
            break;
        }

        int evaRoll = AppUtil::GetRandomInt(0, 99);
        if (evaRoll < pAGI) {
            LOG_INFO("Player Evaded! (Hit {}, Roll: {} < AGI: {})", i + 1, evaRoll, pAGI);
            result.evading = true; // At least one strike evaded
            continue;
        }

        result.totalDamage += eDmgBase;

        int statusRoll = AppUtil::GetRandomInt(0, 99);
        if (isWeak && statusRoll < 1) {
            LOG_INFO("Player Weakened! (Roll: {} < 1)", statusRoll);
            result.weakened = true;
        }
        if (isPoison && statusRoll < 1) {
            LOG_INFO("Player Poisoned! (Roll: {} < 1)", statusRoll);
            result.poisoned = true;
        }
    }

    if (result.totalDamage > 0 || !result.evading) {
        player->ApplyEffect(AppUtil::Effect::HP, -result.totalDamage);
    }
    
    if (player->GetAttr(AppUtil::Effect::HP) <= 0) {
        result.isBattleEnd = true;
    }

    // Apply status debuffs dynamically if hit
    if (!result.evading || result.totalDamage > 0) {
        if (result.weakened) {
            int currentAtk = player->GetAttr(AppUtil::Effect::ATTACK);
            int currentDef = player->GetAttr(AppUtil::Effect::DEFENSE);
            player->SetAttr(AppUtil::Effect::ATTACK, static_cast<int>(currentAtk * 0.8));
            player->SetAttr(AppUtil::Effect::DEFENSE, static_cast<int>(currentDef * 0.8));
        }
        if (result.poisoned) {
            player->SetAttr(AppUtil::Effect::POISON, player->GetAttr(AppUtil::Effect::POISON) + 1);
        }
    }

    return result;
}
