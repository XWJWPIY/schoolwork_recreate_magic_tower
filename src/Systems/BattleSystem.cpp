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
        // 衰弱狀態：攻擊力臨時乘以 0.8，不改儲存值
        if (player->GetIsWeak()) pAtk = static_cast<int>(pAtk * 0.8);
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

BattleSystem::TurnResult BattleSystem::ProcessSingleEnemyHit(
    std::shared_ptr<Player> player, std::shared_ptr<Enemy> enemy) {

    TurnResult result;
    if (!player || !enemy) return result;

    auto meta      = AppUtil::GlobalObjectRegistry[enemy->GetObjectId()];
    bool ignoreDef = meta.GetInt("Ignore_DEF") > 0;
    int  pAGI      = player->GetAttr(AppUtil::Effect::AGILITY);

    std::string specialStr = meta.GetString("Special");
    bool isKilling = meta.GetInt("Killing_ATK") > 0
                  || specialStr == AppUtil::GetGlobalString("battle_special_kill", "Critical");
    bool isWeak    = meta.GetBool("Weak");
    bool isPoison  = meta.GetBool("Poison");

    int pDef     = player->GetAttr(AppUtil::Effect::DEFENSE);
    // 衰弱狀態：防禦力臨時乘以 0.8，不改儲存值
    if (player->GetIsWeak()) pDef = static_cast<int>(pDef * 0.8);
    int eAtk     = enemy->GetAttr(AppUtil::Effect::ATTACK);
    int eDmgBase = ignoreDef ? eAtk : (eAtk - pDef);
    if (eDmgBase < 1) eDmgBase = 1;

    // ── 1. InstantKill 判定 ──────────────────────────────────────────
    int killRoll = AppUtil::GetRandomInt(0, 99);
    if (isKilling && killRoll < 10) {
        result.totalDamage = player->GetAttr(AppUtil::Effect::HP);
        result.instantKill = true;
        result.isBattleEnd = true;
        player->ApplyEffect(AppUtil::Effect::HP, -result.totalDamage);
        LOG_INFO("Instant Kill! (Roll: {} < 10)", killRoll);
        return result;
    }

    // ── 2. 玩家閃避判定（AGI 值 = 閃避機率 %）──────────────────────
    int evaRoll = AppUtil::GetRandomInt(0, 99);
    if (evaRoll < pAGI) {
        result.evading     = true;
        result.totalDamage = 0;
        LOG_INFO("Player Evaded! (Roll: {} < AGI: {})", evaRoll, pAGI);
        return result; // 閃避成功：不扣血、不觸發狀態異常
    }

    // ── 3. 命中：計算傷害並立即扣血 ─────────────────────────────────
    result.totalDamage = eDmgBase;
    player->ApplyEffect(AppUtil::Effect::HP, -result.totalDamage);
    LOG_INFO("Enemy Hit! Damage={} (ignoreDef={})", result.totalDamage, ignoreDef ? 1 : 0);

    // ── 4. 狀態異常判定（各 9% 機率）────────────────────────────────
    int statusRoll = AppUtil::GetRandomInt(0, 99);
    if (isWeak) {
        if (statusRoll < 9) {
            result.weakened = true;
            player->SetIsWeak(true);
            LOG_INFO("Weak Roll: {} / threshold: 9 -> WEAKENED", statusRoll);
        } else {
            LOG_INFO("Weak Roll: {} / threshold: 9 -> miss", statusRoll);
        }
    }

    statusRoll = AppUtil::GetRandomInt(0, 99);
    if (isPoison && statusRoll < 9) {
        result.poisoned = true;
        LOG_INFO("Player Poisoned! (Roll: {} < 9)", statusRoll);
        player->SetAttr(AppUtil::Effect::POISON,
            player->GetAttr(AppUtil::Effect::POISON) + 1);
    }

    // ── 5. 死亡判定 ──────────────────────────────────────────────────
    if (player->GetAttr(AppUtil::Effect::HP) <= 0) {
        result.isBattleEnd = true;
    }

    return result;
}

BattleSystem::TurnResult BattleSystem::ProcessEnemyTurn(
    std::shared_ptr<Player> player, std::shared_ptr<Enemy> enemy) {

    TurnResult combined;
    if (!player || !enemy) return combined;

    auto meta    = AppUtil::GlobalObjectRegistry[enemy->GetObjectId()];
    int  atkTime = meta.GetInt("ATK_Time", 1);
    LOG_INFO("Enemy (ID: {}) Turn Start (atkTime={})", enemy->GetObjectId(), atkTime);

    for (int i = 0; i < atkTime; ++i) {
        auto r = ProcessSingleEnemyHit(player, enemy);
        combined.totalDamage += r.totalDamage;
        combined.evading     |= r.evading;
        combined.isBattleEnd |= r.isBattleEnd;
        combined.instantKill |= r.instantKill;
        combined.weakened    |= r.weakened;
        combined.poisoned    |= r.poisoned;
        if (r.isBattleEnd) break;
    }

    return combined;
}

