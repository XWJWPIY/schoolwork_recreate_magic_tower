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

    // Floor Display: {150, 335}, size 32 (from App.cpp)
    InitNumericText(m_floor_text, 150.0f, 335.0f, Util::Color::FromRGB(255, 255, 255), 32);
    m_floor_text->SetSuffix(" F");
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
        m_yellow_key_text->SetNumber(player->GetYellowKeys());
        m_blue_key_text->SetNumber(player->GetBlueKeys());
        m_red_key_text->SetNumber(player->GetRedKeys());

        m_yellow_key_text->UpdateDisplayText();
        m_blue_key_text->UpdateDisplayText();
        m_red_key_text->UpdateDisplayText();
    }

    m_floor_text->SetNumber(floorNum);
    m_floor_text->UpdateDisplayText();
}

void StatusUI::SetVisible(bool visible) {
    m_yellow_key_text->SetVisible(visible);
    m_blue_key_text->SetVisible(visible);
    m_red_key_text->SetVisible(visible);
    m_floor_text->SetVisible(visible);
}

void StatusUI::AddToRoot(Util::Renderer& root) {
    root.AddChild(m_yellow_key_text);
    root.AddChild(m_blue_key_text);
    root.AddChild(m_red_key_text);
    root.AddChild(m_floor_text);
}
