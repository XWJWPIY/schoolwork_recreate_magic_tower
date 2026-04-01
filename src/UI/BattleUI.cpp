#include "UI/BattleUI.hpp"
#include "Core/AppUtil.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Time.hpp"
#include "Util/Logger.hpp"
#include "Systems/BattleSystem.hpp"
#include <algorithm>

namespace {
    const glm::vec2 BG_POS = {141.0f, 25.0f};
    const float Z_BG = 20.0f;
    const float Z_AVATAR = 21.0f;
    const float Z_TEXT = 22.0f;
    const float TURN_SPEED_MS = 300.0f; 

    const Util::Color CLR_WHITE = Util::Color::FromRGB(255, 255, 255);
    const Util::Color CLR_RED   = Util::Color::FromRGB(255, 0, 0);
    const Util::Color CLR_GREEN = Util::Color::FromRGB(0, 255, 0);
    const Util::Color CLR_YELLOW= Util::Color::FromRGB(255, 255, 0);
}

BattleUI::BattleUI(const std::string& fontPath) {
    m_background = std::make_shared<Util::GameObject>();
    m_background->SetDrawable(std::make_shared<Util::Image>(AppUtil::GetStaticResourcePath("bmp/Enemy/Fighting.bmp")));
    m_background->m_Transform.translation = BG_POS;
    m_background->m_Transform.scale = {0.75f, 0.75f};
    m_background->SetZIndex(Z_BG);

    m_reward_bg = std::make_shared<Util::GameObject>();
    m_reward_bg->SetDrawable(std::make_shared<Util::Image>(AppUtil::GetStaticResourcePath("bmp/Enemy/Reward.bmp")));
    m_reward_bg->m_Transform.translation = BG_POS + glm::vec2(0.0f, -142.0f);
    m_reward_bg->m_Transform.scale = {0.75f, 0.75f};
    m_reward_bg->SetZIndex(Z_BG);

    auto makeText = [&](float x, float y, const Util::Color& clr, NumericDisplayText::Align align = NumericDisplayText::Align::LEFT) {
        auto t = std::make_shared<NumericDisplayText>(fontPath, 24);
        t->SetAlignment(align);
        t->m_Transform.translation = {x, y};
        t->SetColor(clr);
        t->SetZIndex(Z_TEXT);
        return t;
    };

    // Enemy Layout (Left)
    float e_x = BG_POS.x - 250.0f;
    float p_x = BG_POS.x + 150.0f;

    m_enemy_name = makeText(e_x - 30.0f, BG_POS.y + 80.0f, CLR_WHITE, NumericDisplayText::Align::LEFT); 
    m_enemy_name->SetShowNumber(false);
    
    m_enemy_avatar = std::make_shared<Util::GameObject>();
    m_enemy_avatar->m_Transform.translation = {e_x - 9.0f, BG_POS.y + 28.0f};
    m_enemy_avatar->m_Transform.scale = {0.735f, 0.735f};
    m_enemy_avatar->SetZIndex(Z_AVATAR);

    m_enemy_hp  = makeText(e_x + 60.0f, BG_POS.y + 40.0f,  CLR_WHITE); m_enemy_hp->SetPrefix(AppUtil::GetGlobalString("battle_enemy_hp", "HP: "));
    m_enemy_atk = makeText(e_x + 60.0f, BG_POS.y +  0.0f,  CLR_WHITE); m_enemy_atk->SetPrefix(AppUtil::GetGlobalString("battle_enemy_atk", "ATK: "));
    m_enemy_def = makeText(e_x + 60.0f, BG_POS.y - 40.0f,  CLR_WHITE); m_enemy_def->SetPrefix(AppUtil::GetGlobalString("battle_enemy_def", "DEF: "));
    m_enemy_agi = makeText(e_x + 60.0f, BG_POS.y - 80.0f,  CLR_WHITE); m_enemy_agi->SetPrefix(AppUtil::GetGlobalString("battle_enemy_agi", "AGI: "));

    // Player Layout (Right)
    m_player_name = makeText(p_x + 150.0f, BG_POS.y + 80.0f, CLR_WHITE, NumericDisplayText::Align::RIGHT); 
    m_player_name->SetShowNumber(false); 
    m_player_name->SetPrefix(AppUtil::GetGlobalString("battle_player_name", "Hero"));
    m_player_name->UpdateDisplayText();

    m_player_avatar = std::make_shared<Util::GameObject>();
    m_player_avatar->m_Transform.translation = {p_x + 108.0f, BG_POS.y + 28.0f};
    m_player_avatar->m_Transform.scale = {0.735f, 0.735f};
    m_player_avatar->SetZIndex(Z_AVATAR);
    m_player_avatar->SetDrawable(std::make_shared<Util::Image>(AppUtil::GetStaticResourcePath("bmp/Player/player_11.bmp")));

    m_player_hp  = makeText(p_x + 60.0f, BG_POS.y + 40.0f,  CLR_WHITE, NumericDisplayText::Align::RIGHT); m_player_hp->SetSuffix(AppUtil::GetGlobalString("battle_player_hp", " :HP"));
    m_player_atk = makeText(p_x + 60.0f, BG_POS.y +  0.0f,  CLR_WHITE, NumericDisplayText::Align::RIGHT); m_player_atk->SetSuffix(AppUtil::GetGlobalString("battle_player_atk", " :ATK"));
    m_player_def = makeText(p_x + 60.0f, BG_POS.y - 40.0f,  CLR_WHITE, NumericDisplayText::Align::RIGHT); m_player_def->SetSuffix(AppUtil::GetGlobalString("battle_player_def", " :DEF"));
    m_player_agi = makeText(p_x + 60.0f, BG_POS.y - 80.0f,  CLR_WHITE, NumericDisplayText::Align::RIGHT); m_player_agi->SetSuffix(AppUtil::GetGlobalString("battle_player_agi", " :AGI"));

    m_vs_text = makeText(BG_POS.x, BG_POS.y + 80.0f, CLR_WHITE, NumericDisplayText::Align::CENTER);
    m_vs_text->SetPrefix(AppUtil::GetGlobalString("battle_vs", "VS")); 
    m_vs_text->SetShowNumber(false);
    m_vs_text->UpdateDisplayText();

    m_hint_text = makeText(BG_POS.x + 350.0f, BG_POS.y - 80.0f, CLR_YELLOW, NumericDisplayText::Align::RIGHT);
    m_hint_text->SetPrefix(AppUtil::GetGlobalString("battle_retreat", "Exit(Q)")); 
    m_hint_text->SetShowNumber(false);
    m_hint_text->UpdateDisplayText();

    // Floating text for damage
    m_floating_text = makeText(0, 0, CLR_RED, NumericDisplayText::Align::CENTER);
    m_floating_text->SetShowNumber(false);
    m_floating_text->SetZIndex(Z_TEXT + 1.0f);

    // Reward elements
    m_reward_text1 = makeText(BG_POS.x - 250.0f, BG_POS.y -140.0f, CLR_WHITE, NumericDisplayText::Align::CENTER); 
    m_reward_text1->SetShowNumber(false); m_reward_text1->SetZIndex(Z_BG + 6.0f);
    m_reward_text2 = makeText(BG_POS.x + 20.0f, BG_POS.y - 140.0f, CLR_WHITE, NumericDisplayText::Align::CENTER); 
    m_reward_text2->SetShowNumber(false); m_reward_text2->SetZIndex(Z_BG + 6.0f);
    m_reward_hint = makeText(BG_POS.x + 280.0f, BG_POS.y - 140.0f, CLR_WHITE, NumericDisplayText::Align::CENTER);
    m_reward_hint->SetShowNumber(false); m_reward_hint->SetZIndex(Z_BG + 6.0f);

    SetVisible(false);
}

void BattleUI::Start(std::shared_ptr<Player> player, std::shared_ptr<Enemy> enemy, std::function<void(bool)> onEnd) {
    m_player = player;
    m_enemy = enemy;
    m_on_end = onEnd;
    m_player_turn = true;
    m_turn_timer = 0.0f;
    m_state = State::FIGHTING;

    auto meta = AppUtil::GlobalObjectRegistry[enemy->GetObjectId()];
    m_enemy_name->SetPrefix(meta.GetString(AppUtil::Attr::TITLE));
    m_enemy_name->UpdateDisplayText();

    m_enemy_avatar->SetDrawable(std::make_shared<Util::Image>(AppUtil::GetFullResourcePath(meta.GetInt("Icon_ID", enemy->GetObjectId()))));

    m_floating_text->SetVisible(false);
    m_reward_bg->SetVisible(false);
    m_reward_bg->SetVisible(false);
    m_reward_text1->SetVisible(false);
    m_reward_text2->SetVisible(false);
    m_reward_hint->SetVisible(false);

    SetVisible(true);
    RefreshStats();
}

void BattleUI::SetAnimation(bool isPlayerTurn, int damage) {
    m_floating_text->SetVisible(true);
    m_floating_text->SetPrefix("-" + std::to_string(damage));
    m_floating_text->UpdateDisplayText();
    
    // If it's Player's turn, damage displays on Enemy, else on Player
    if (isPlayerTurn) {
        m_floating_text->m_Transform.translation = m_enemy_avatar->m_Transform.translation + glm::vec2(0.0f, -40.0f);
    } else {
        m_floating_text->m_Transform.translation = m_player_avatar->m_Transform.translation + glm::vec2(0.0f, -40.0f);
    }
}

void BattleUI::RefreshStats() {
    if (!m_player || !m_enemy) return;

    m_player_hp->SetNumber(std::max(0, m_player->GetAttr(AppUtil::Effect::HP))); m_player_hp->UpdateDisplayText();
    m_player_atk->SetNumber(m_player->GetAttr(AppUtil::Effect::ATTACK)); m_player_atk->UpdateDisplayText();
    m_player_def->SetNumber(m_player->GetAttr(AppUtil::Effect::DEFENSE)); m_player_def->UpdateDisplayText();
    m_player_agi->SetNumber(m_player->GetAttr(AppUtil::Effect::AGILITY)); m_player_agi->UpdateDisplayText();

    m_enemy_hp->SetNumber(std::max(0, m_enemy->GetAttr(AppUtil::Effect::HP))); m_enemy_hp->UpdateDisplayText();
    m_enemy_atk->SetNumber(m_enemy->GetAttr(AppUtil::Effect::ATTACK)); m_enemy_atk->UpdateDisplayText();
    m_enemy_def->SetNumber(m_enemy->GetAttr(AppUtil::Effect::DEFENSE)); m_enemy_def->UpdateDisplayText();
    m_enemy_agi->SetNumber(m_enemy->GetAttr(AppUtil::Effect::AGILITY)); m_enemy_agi->UpdateDisplayText();
}

void BattleUI::run() {
    if (!m_visible) return;

    if (Util::Input::IsKeyDown(Util::Keycode::I)) {
        m_is_frozen = !m_is_frozen;
        LOG_INFO("Battle Frozen: %s", m_is_frozen ? "TRUE" : "FALSE");
    }

    if (m_is_frozen) return;

    if (m_state == State::REWARD) {
        m_reward_hint->SetVisible(((static_cast<int>(Util::Time::GetElapsedTimeMs()) / 500) % 2) == 0);
        if (Util::Input::IsKeyDown(Util::Keycode::SPACE)) {
            SetVisible(false);
            if (m_on_end) m_on_end(true); // Win confirmed
        }
        return;
    }

    if (Util::Input::IsKeyDown(Util::Keycode::Q)) {
        LOG_INFO("Battle Retreat!");
        SetVisible(false);
        if (m_on_end) m_on_end(false); // Retreat
        return;
    }

    m_turn_timer += Util::Time::GetDeltaTimeMs();
    if (m_turn_timer >= TURN_SPEED_MS) {
        m_turn_timer = 0.0f;
        LOG_INFO("DEBUG: Turning to {}", m_player_turn ? "PLAYER" : "ENEMY");

        if (m_player_turn) {
            auto result = BattleSystem::ProcessPlayerTurn(m_player, m_enemy);
            
            SetAnimation(true, result.totalDamage);

            if (result.isBattleEnd) {
                RefreshStats(); // Ensure the HP: 0 is reflected in UI
                m_state = State::REWARD;
                m_reward_bg->SetVisible(true);
                m_reward_text1->SetPrefix(AppUtil::GetGlobalString("battle_win", "Victory!")); 
                m_reward_text1->UpdateDisplayText(); 
                m_reward_text1->SetVisible(true);
                m_reward_text2->SetPrefix(AppUtil::GetGlobalString("battle_exp", "EXP: ") + std::to_string(result.rewardExp) + AppUtil::GetGlobalString("battle_coin", " Coin: ") + std::to_string(result.rewardCoin));
                m_reward_text2->UpdateDisplayText(); 
                m_reward_text2->SetVisible(true);
                m_reward_hint->SetPrefix(AppUtil::GetGlobalString("battle_reward_hint", "-SPACE-"));
                m_reward_hint->UpdateDisplayText();
                m_reward_hint->SetVisible(true);
                m_floating_text->SetVisible(false);
            }
        } else {
            auto result = BattleSystem::ProcessEnemyTurn(m_player, m_enemy);
            
            SetAnimation(false, result.totalDamage);

            if (result.isBattleEnd) {
                SetVisible(false);
                if (m_on_end) m_on_end(false); // Dead
                return;
            }
        }

        if (m_state != State::REWARD) {
            m_player_turn = !m_player_turn;
            RefreshStats();
        }
    }
}

void BattleUI::SetVisible(bool visible) {
    m_visible = visible;
    if (m_background) m_background->SetVisible(visible && m_state == State::FIGHTING);
    if (m_player_avatar) m_player_avatar->SetVisible(visible && m_state == State::FIGHTING);
    if (m_player_hp) m_player_hp->SetVisible(visible && m_state == State::FIGHTING);
    if (m_player_atk) m_player_atk->SetVisible(visible && m_state == State::FIGHTING);
    if (m_player_def) m_player_def->SetVisible(visible && m_state == State::FIGHTING);
    if (m_player_agi) m_player_agi->SetVisible(visible && m_state == State::FIGHTING);
    if (m_player_name) m_player_name->SetVisible(visible && m_state == State::FIGHTING);
    
    if (m_enemy_avatar) m_enemy_avatar->SetVisible(visible && m_state == State::FIGHTING);
    if (m_enemy_name) m_enemy_name->SetVisible(visible && m_state == State::FIGHTING);
    if (m_enemy_hp) m_enemy_hp->SetVisible(visible && m_state == State::FIGHTING);
    if (m_enemy_atk) m_enemy_atk->SetVisible(visible && m_state == State::FIGHTING);
    if (m_enemy_def) m_enemy_def->SetVisible(visible && m_state == State::FIGHTING);
    if (m_enemy_agi) m_enemy_agi->SetVisible(visible && m_state == State::FIGHTING);

    if (m_vs_text) m_vs_text->SetVisible(visible && m_state == State::FIGHTING);
    if (m_hint_text) m_hint_text->SetVisible(visible && m_state == State::FIGHTING);
    
    if (!visible) {
        if (m_floating_text) m_floating_text->SetVisible(false);
        if (m_reward_bg) m_reward_bg->SetVisible(false);
        if (m_reward_text1) m_reward_text1->SetVisible(false);
        if (m_reward_text2) m_reward_text2->SetVisible(false);
        if (m_reward_hint) m_reward_hint->SetVisible(false);
    }
}

void BattleUI::AddToRoot(Util::Renderer& root) {
    if (m_background) root.AddChild(m_background);
    if (m_reward_bg) root.AddChild(m_reward_bg);
    if (m_reward_text1) root.AddChild(m_reward_text1);
    if (m_reward_text2) root.AddChild(m_reward_text2);
    if (m_reward_hint) root.AddChild(m_reward_hint);

    if (m_player_avatar) root.AddChild(m_player_avatar);
    if (m_player_name) root.AddChild(m_player_name);
    if (m_player_hp) root.AddChild(m_player_hp);
    if (m_player_atk) root.AddChild(m_player_atk);
    if (m_player_def) root.AddChild(m_player_def);
    if (m_player_agi) root.AddChild(m_player_agi);

    if (m_enemy_avatar) root.AddChild(m_enemy_avatar);
    if (m_enemy_name) root.AddChild(m_enemy_name);
    if (m_enemy_hp) root.AddChild(m_enemy_hp);
    if (m_enemy_atk) root.AddChild(m_enemy_atk);
    if (m_enemy_def) root.AddChild(m_enemy_def);
    if (m_enemy_agi) root.AddChild(m_enemy_agi);

    if (m_vs_text) root.AddChild(m_vs_text);
    if (m_hint_text) root.AddChild(m_hint_text);
    if (m_floating_text) root.AddChild(m_floating_text);
}
