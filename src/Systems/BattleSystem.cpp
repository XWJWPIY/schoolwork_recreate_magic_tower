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
    bool enemyEvaded = AppUtil::CheckProbability("Player's Turn", "Evasion", eAGI);

    if (enemyEvaded) {
        // Enemy evaded
        result.evading = true;
        result.totalDamage = 0;
    } else {
        // Player hits
        int eDef = enemy->GetAttr(AppUtil::Effect::DEFENSE);
        int pAtk = player->GetAttr(AppUtil::Effect::ATTACK);
        // 衰弱狀態：攻擊力臨時乘以 0.8，不改儲存值
        if (player->GetIsWeak()) pAtk = static_cast<int>(pAtk * 0.8);
        int dmg = std::max(1, pAtk - eDef); // Damage floor 1

        enemy->ApplyEffect(AppUtil::Effect::HP, -dmg);
        result.totalDamage = dmg;
    }

    if (enemy->GetAttr(AppUtil::Effect::HP) <= 0) {
        result.isBattleEnd = true;
        auto it = AppUtil::GlobalObjectRegistry.find(enemy->GetObjectId());
        if (it != AppUtil::GlobalObjectRegistry.end()) {
            result.rewardExp  = it->second.GetInt(AppUtil::Attr::EXP);
            result.rewardCoin = it->second.GetInt(AppUtil::Attr::COIN);
        }
    }

    return result;
}

BattleSystem::TurnResult BattleSystem::ProcessSingleEnemyHit(
    std::shared_ptr<Player> player, std::shared_ptr<Enemy> enemy) {

    TurnResult result;
    if (!player || !enemy) return result;

    auto it = AppUtil::GlobalObjectRegistry.find(enemy->GetObjectId());
    if (it == AppUtil::GlobalObjectRegistry.end()) return result;
    const auto& meta = it->second;
    bool ignoreDef = meta.GetInt("Ignore_DEF") > 0;
    int  pAGI      = player->GetAttr(AppUtil::Effect::AGILITY);

    int killRate   = meta.GetInt("Killing_ATK", 0);
    int weakRate   = meta.GetInt("Weak", 0);
    int poisonRate = meta.GetInt("Poison", 0);

    int pDef     = player->GetAttr(AppUtil::Effect::DEFENSE);
    // 衰弱狀態：防禦力臨時乘以 0.8，不改儲存值
    if (player->GetIsWeak()) pDef = static_cast<int>(pDef * 0.8);
    int eAtk     = enemy->GetAttr(AppUtil::Effect::ATTACK);
    int eDmgBase = ignoreDef ? eAtk : (eAtk - pDef);
    if (eDmgBase < 1) eDmgBase = 1;

    // ── 1. 玩家閃避判定（AGI 值 = 閃避機率 %）──────────────────────
    if (AppUtil::CheckProbability("Enemy's Turn", "Evasion", pAGI)) {
        result.evading     = true;
        result.totalDamage = 0;
        return result; // 閃避成功：跳過後續所有判定、不扣血、不觸發狀態異常
    }

    // ── 2. InstantKill 判定 ──────────────────────────────────────────
    if (AppUtil::CheckProbability("Enemy's Turn", "Critical", killRate)) {
        result.totalDamage = player->GetAttr(AppUtil::Effect::HP);
        result.instantKill = true;
        result.isBattleEnd = true;
        player->ApplyEffect(AppUtil::Effect::HP, -result.totalDamage);
        return result;
    }

    // ── 3. 命中：計算傷害並立即扣血 ─────────────────────────────────
    result.totalDamage = eDmgBase;
    player->ApplyEffect(AppUtil::Effect::HP, -result.totalDamage);
    LOG_INFO("Enemy Hit! Damage={} (ignoreDef={})", result.totalDamage, ignoreDef);

    // ── 4. 狀態異常判定 ──────────────────────────────────────────────
    if (AppUtil::CheckProbability("Enemy's Turn", "Weak Strike", weakRate)) {
        result.weakened = true;
        player->SetIsWeak(true);
    }

    if (AppUtil::CheckProbability("Enemy's Turn", "Poison Strike", poisonRate)) {
        result.poisoned = true;
        player->SetIsPoison(true);
    }

    // ── 5. 死亡判定 ──────────────────────────────────────────────────
    if (player->GetAttr(AppUtil::Effect::HP) <= 0) {
        result.isBattleEnd = true;
    }

    return result;
}



