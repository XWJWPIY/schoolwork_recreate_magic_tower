#ifndef STATUS_UI_HPP
#define STATUS_UI_HPP

#include "NumericDisplayText.hpp"
#include "Player.hpp"
#include "Util/Renderer.hpp"

class StatusUI {
public:
    StatusUI(unsigned int fontSize = 36);
    ~StatusUI() = default;

    void Update(const std::shared_ptr<Player>& player, int floorNum);
    void SetVisible(bool visible);
    
    // Getter for the root group if needed, or we can just pass the root to it
    void AddToRoot(Util::Renderer& root);

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
    
    unsigned int m_default_font_size;

    // Helper to initialize text objects
    void InitNumericText(std::shared_ptr<NumericDisplayText>& text, 
                         float x, float y, const Util::Color& color, unsigned int size);
};

#endif // STATUS_UI_HPP
