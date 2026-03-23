#include "StatusUI.hpp"
#include "AppUtil.hpp"

StatusUI::StatusUI(unsigned int fontSize) : m_default_font_size(fontSize) {
    // Current layout based on manual adjustments in App.cpp
    // Yellow: -290, -115
    // Blue:   -290, -172
    // Red:    -290, -228
    
    InitNumericText(m_yellow_key_text, -290.0f, -115.0f, Util::Color::FromRGB(255, 255, 255), m_default_font_size);
    InitNumericText(m_blue_key_text, -290.0f, -172.0f, Util::Color::FromRGB(255, 255, 255), m_default_font_size);
    InitNumericText(m_red_key_text, -290.0f, -228.0f, Util::Color::FromRGB(255, 255, 255), m_default_font_size);
    InitNumericText(m_coin_text, -290.0f, -284.0f, Util::Color::FromRGB(255, 255, 255), m_default_font_size);
    
    // Stats Display (Above keys)
    InitNumericText(m_level_text, -277.0f, 209.0f, Util::Color::FromRGB(255, 255, 255), 24);
    InitNumericText(m_hp_text, -277.0f, 166.0f, Util::Color::FromRGB(255, 255, 255), 24);
    InitNumericText(m_attack_text, -277.0f, 124.0f, Util::Color::FromRGB(255, 255, 255), 24);
    InitNumericText(m_defense_text, -277.0f, 83.0f, Util::Color::FromRGB(255, 255, 255), 24);
    InitNumericText(m_agility_text, -277.0f, 41.0f, Util::Color::FromRGB(255, 255, 255), 24);
    InitNumericText(m_exp_text, -277.0f, 0.0f, Util::Color::FromRGB(255, 255, 255), 24);

    // Floor Display: {150, 335}, size 32 (from App.cpp)
    InitNumericText(m_floor_text, 150.0f, 335.0f, Util::Color::FromRGB(255, 255, 255), 32);
    m_floor_text->SetSuffix(" F");

    // Manual Hint
    InitNumericText(m_manual_hint_text, 390.0f, -335.0f, Util::Color::FromRGB(0, 0, 0), 24);
    m_manual_hint_text->SetPrefix("-Press (L)-");
    m_manual_hint_text->SetShowNumber(false);
    m_manual_hint_text->UpdateDisplayText();
}

void StatusUI::InitNumericText(std::shared_ptr<NumericDisplayText>& text, 
                               float x, float y, const Util::Color& color, unsigned int size) {
    text = std::make_shared<NumericDisplayText>(
        MAGIC_TOWER_RESOURCE_DIR "/Font/Cubic_11.ttf", size);
    text->SetPrefix("");
    text->SetNumber(0);
    text->SetColor(color);
    text->m_Transform.translation = {x, y};
    text->SetZIndex(-3.0f);
    text->SetVisible(false);
}

void StatusUI::Update(const std::shared_ptr<Player>& player, int floorNum) {
    if (player) {
        m_yellow_key_text->SetNumber(player->GetAttr(AppUtil::Effect::KEY_YELLOW));
        m_blue_key_text->SetNumber(player->GetAttr(AppUtil::Effect::KEY_BLUE));
        m_red_key_text->SetNumber(player->GetAttr(AppUtil::Effect::KEY_RED));
        m_coin_text->SetNumber(player->GetAttr(AppUtil::Effect::COIN));
        
        m_level_text->SetNumber(player->GetAttr(AppUtil::Effect::LEVEL));
        m_hp_text->SetNumber(player->GetAttr(AppUtil::Effect::HP));
        m_attack_text->SetNumber(player->GetAttr(AppUtil::Effect::ATTACK));
        m_defense_text->SetNumber(player->GetAttr(AppUtil::Effect::DEFENSE));
        m_agility_text->SetNumber(player->GetAttr(AppUtil::Effect::AGILITY));
        m_exp_text->SetNumber(player->GetAttr(AppUtil::Effect::EXP));

        m_yellow_key_text->UpdateDisplayText();
        m_blue_key_text->UpdateDisplayText();
        m_red_key_text->UpdateDisplayText();
        m_coin_text->UpdateDisplayText();
        
        m_level_text->UpdateDisplayText();
        m_hp_text->UpdateDisplayText();
        m_attack_text->UpdateDisplayText();
        m_defense_text->UpdateDisplayText();
        m_agility_text->UpdateDisplayText();
        m_exp_text->UpdateDisplayText();
    }

    m_floor_text->SetNumber(floorNum);
    m_floor_text->UpdateDisplayText();
}

void StatusUI::SetVisible(bool visible) {
    m_yellow_key_text->SetVisible(visible);
    m_blue_key_text->SetVisible(visible);
    m_red_key_text->SetVisible(visible);
    m_coin_text->SetVisible(visible);
    m_level_text->SetVisible(visible);
    m_hp_text->SetVisible(visible);
    m_attack_text->SetVisible(visible);
    m_defense_text->SetVisible(visible);
    m_agility_text->SetVisible(visible);
    m_exp_text->SetVisible(visible);
    m_floor_text->SetVisible(visible);
    m_manual_hint_text->SetVisible(visible);
}

void StatusUI::AddToRoot(Util::Renderer& root) {
    root.AddChild(m_yellow_key_text);
    root.AddChild(m_blue_key_text);
    root.AddChild(m_red_key_text);
    root.AddChild(m_coin_text);
    root.AddChild(m_level_text);
    root.AddChild(m_hp_text);
    root.AddChild(m_attack_text);
    root.AddChild(m_defense_text);
    root.AddChild(m_agility_text);
    root.AddChild(m_exp_text);
    root.AddChild(m_floor_text);
    root.AddChild(m_manual_hint_text);
}
