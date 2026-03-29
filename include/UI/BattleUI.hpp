#ifndef BATTLE_UI_HPP
#define BATTLE_UI_HPP

#include "UI/UIComponent.hpp"
#include "UI/NumericDisplayText.hpp"
#include "Objects/Player.hpp"
#include "Objects/Enemy.hpp"
#include "Util/Image.hpp"
#include <memory>
#include <functional>

class BattleUI : public UIComponent {
public:
    BattleUI(const std::string& fontPath);

    void Start(std::shared_ptr<Player> player, std::shared_ptr<Enemy> enemy, std::function<void(bool)> onEnd);

    void run() override;
    bool IsIntercepting() const override { return m_visible; }
    bool IsActive() const override { return m_visible; }
    void SetVisible(bool visible) override;
    void AddToRoot(Util::Renderer& root) override;

private:
    void RefreshStats();
    void SetAnimation(bool isPlayerTurn, int damage);

    std::shared_ptr<Player> m_player;
    std::shared_ptr<Enemy> m_enemy;
    std::function<void(bool)> m_on_end;

    float m_turn_timer = 0.0f;
    bool m_player_turn = true;
    bool m_is_frozen = false;

    enum class State {
        FIGHTING,
        REWARD
    };
    State m_state = State::FIGHTING;

    std::shared_ptr<Util::GameObject> m_background;

    // Enemy Stats (Left)
    std::shared_ptr<Util::GameObject> m_enemy_avatar;
    std::shared_ptr<NumericDisplayText> m_enemy_name;
    std::shared_ptr<NumericDisplayText> m_enemy_hp;
    std::shared_ptr<NumericDisplayText> m_enemy_atk;
    std::shared_ptr<NumericDisplayText> m_enemy_def;
    std::shared_ptr<NumericDisplayText> m_enemy_agi;

    // Player Stats (Right)
    std::shared_ptr<Util::GameObject> m_player_avatar;
    std::shared_ptr<NumericDisplayText> m_player_name;
    std::shared_ptr<NumericDisplayText> m_player_hp;
    std::shared_ptr<NumericDisplayText> m_player_atk;
    std::shared_ptr<NumericDisplayText> m_player_def;
    std::shared_ptr<NumericDisplayText> m_player_agi;

    std::shared_ptr<NumericDisplayText> m_vs_text;
    std::shared_ptr<NumericDisplayText> m_hint_text;

    // Animations
    std::shared_ptr<NumericDisplayText> m_floating_text;

    // Reward Panel
    std::shared_ptr<Util::GameObject> m_reward_bg;
    std::shared_ptr<NumericDisplayText> m_reward_text1;
    std::shared_ptr<NumericDisplayText> m_reward_text2;
};

#endif // BATTLE_UI_HPP
