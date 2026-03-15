#include "StatusUI.hpp"
#include "AppUtil.hpp"

StatusUI::StatusUI(unsigned int fontSize) : m_DefaultFontSize(fontSize) {
    // Current layout based on manual adjustments in App.cpp
    // Yellow: -290, -115
    // Blue:   -290, -172
    // Red:    -290, -228
    
    InitNumericText(m_YellowKeyText, -290.0f, -115.0f, Util::Color::FromRGB(255, 255, 255), m_DefaultFontSize);
    InitNumericText(m_BlueKeyText, -290.0f, -172.0f, Util::Color::FromRGB(255, 255, 255), m_DefaultFontSize);
    InitNumericText(m_RedKeyText, -290.0f, -228.0f, Util::Color::FromRGB(255, 255, 255), m_DefaultFontSize);

    // Floor Display: {150, 335}, size 32 (from App.cpp)
    InitNumericText(m_FloorText, 150.0f, 335.0f, Util::Color::FromRGB(255, 255, 255), 32);
    m_FloorText->SetSuffix(" F");
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
        m_YellowKeyText->SetNumber(player->GetYellowKeys());
        m_BlueKeyText->SetNumber(player->GetBlueKeys());
        m_RedKeyText->SetNumber(player->GetRedKeys());

        m_YellowKeyText->UpdateDisplayText();
        m_BlueKeyText->UpdateDisplayText();
        m_RedKeyText->UpdateDisplayText();
    }

    m_FloorText->SetNumber(floorNum);
    m_FloorText->UpdateDisplayText();
}

void StatusUI::SetVisible(bool visible) {
    m_YellowKeyText->SetVisible(visible);
    m_BlueKeyText->SetVisible(visible);
    m_RedKeyText->SetVisible(visible);
    m_FloorText->SetVisible(visible);
}

void StatusUI::AddToRoot(Util::Renderer& root) {
    root.AddChild(m_YellowKeyText);
    root.AddChild(m_BlueKeyText);
    root.AddChild(m_RedKeyText);
    root.AddChild(m_FloorText);
}
