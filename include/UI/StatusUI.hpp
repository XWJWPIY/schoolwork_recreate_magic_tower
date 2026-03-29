#ifndef STATUS_UI_HPP
#define STATUS_UI_HPP

#include "UI/NumericDisplayText.hpp"
#include "Objects/Player.hpp"
#include "Util/Renderer.hpp"
#include "UI/UIComponent.hpp"
#include "Core/FloorMap.hpp"
#include <memory>

class StatusUI : public UIComponent {
public:
    StatusUI(const std::shared_ptr<Player>& player, 
             const std::shared_ptr<FloorMap>& floorMap,
             unsigned int fontSize = 36);
    virtual ~StatusUI() = default;

    // UIComponent Interface
    void run() override;
    void SetVisible(bool visible) override;
    void AddToRoot(Util::Renderer& root) override;
    bool IsActive() const override { return m_visible; }
    bool IsIntercepting() const override { return false; } // Never blocks movement

    void Update(const std::shared_ptr<Player>& player, int floorNum);

private:
    std::shared_ptr<NumericDisplayText> m_yellow_key_text;
    std::shared_ptr<NumericDisplayText> m_blue_key_text;
    std::shared_ptr<NumericDisplayText> m_red_key_text;
    std::shared_ptr<NumericDisplayText> m_coin_text;
    std::shared_ptr<NumericDisplayText> m_level_text;
    std::shared_ptr<NumericDisplayText> m_hp_text;
    std::shared_ptr<NumericDisplayText> m_attack_text;
    std::shared_ptr<NumericDisplayText> m_defense_text;
    std::shared_ptr<NumericDisplayText> m_agility_text;
    std::shared_ptr<NumericDisplayText> m_exp_text;
    std::shared_ptr<NumericDisplayText> m_floor_text;
    std::shared_ptr<NumericDisplayText> m_manual_hint_text;
    
    unsigned int m_default_font_size;

    std::shared_ptr<Player> m_player;
    std::shared_ptr<FloorMap> m_road_map;

    // Helper to initialize text objects
    void InitNumericText(std::shared_ptr<NumericDisplayText>& text, 
                         float x, float y, const Util::Color& color, unsigned int size);
};

#endif // STATUS_UI_HPP
