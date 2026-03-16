#include "FlyUI.hpp"
#include "AppUtil.hpp"

FlyUI::FlyUI() {
    m_background = std::make_shared<Util::GameObject>();
    m_background->SetDrawable(std::make_shared<Util::Image>(MAGIC_TOWER_RESOURCE_DIR "/bmp/Special/Fly.bmp"));
    
    // Position and scale same as NoticeUI
    m_background->m_Transform.translation = {141.0f, 0.0f};
    m_background->m_Transform.scale = {0.735f, 0.735f};
    
    // Higher Z-index to be on top of everything (Road -5, Things -4, Player -3)
    m_background->SetZIndex(90.0f);
    
    // Floor Text Setup
    m_floor_text = std::make_shared<NumericDisplayText>(MAGIC_TOWER_RESOURCE_DIR "/Font/Cubic_11.ttf", 81);
    m_floor_text->SetPrefix("");
    m_floor_text->SetNumber(0);
    m_floor_text->SetColor(Util::Color::FromRGB(255, 255, 255));
    // Center it relative to the FlyUI background
    m_floor_text->m_Transform.translation = {141.0f, 0.0f};
    m_floor_text->SetZIndex(91.0f); // In front of background
    m_floor_text->UpdateDisplayText();

    // Arrows initialization
    m_up_white = std::make_shared<Util::Image>(MAGIC_TOWER_RESOURCE_DIR "/bmp/Special/up_arrow_white.png");
    m_up_gray = std::make_shared<Util::Image>(MAGIC_TOWER_RESOURCE_DIR "/bmp/Special/up_arrow_gray.png");
    m_down_white = std::make_shared<Util::Image>(MAGIC_TOWER_RESOURCE_DIR "/bmp/Special/down_arrow_white.png");
    m_down_gray = std::make_shared<Util::Image>(MAGIC_TOWER_RESOURCE_DIR "/bmp/Special/down_arrow_gray.png");

    m_up_arrow = std::make_shared<Util::GameObject>();
    m_up_arrow->m_Transform.translation = {141.0f, 220.0f};
    m_up_arrow->m_Transform.scale = {0.5f, 0.5f};
    m_up_arrow->SetZIndex(92.0f);
    
    m_down_arrow = std::make_shared<Util::GameObject>();
    m_down_arrow->m_Transform.translation = {141.0f, -220.0f};
    m_down_arrow->m_Transform.scale = {0.5f, 0.5f};
    m_down_arrow->SetZIndex(92.0f);

    // Hints
    m_enter_text = std::make_shared<NumericDisplayText>(MAGIC_TOWER_RESOURCE_DIR "/Font/Cubic_11.ttf", 24);
    m_enter_text->SetPrefix("-Enter-");
    m_enter_text->SetShowNumber(false);
    m_enter_text->SetColor(Util::Color::FromRGB(255, 255, 255));
    m_enter_text->m_Transform.translation = {300.0f, -110.0f};
    m_enter_text->SetZIndex(92.0f);
    m_enter_text->UpdateDisplayText();

    m_quit_text = std::make_shared<NumericDisplayText>(MAGIC_TOWER_RESOURCE_DIR "/Font/Cubic_11.ttf", 24);
    m_quit_text->SetPrefix("-Quit-(F)");
    m_quit_text->SetShowNumber(false);
    m_quit_text->SetColor(Util::Color::FromRGB(255, 255, 255));
    m_quit_text->m_Transform.translation = {350.0f, -250.0f};
    m_quit_text->SetZIndex(92.0f);
    m_quit_text->UpdateDisplayText();
    
    SetVisible(false);
}

void FlyUI::SetVisible(bool visible) {
    m_background->SetVisible(visible);
    m_floor_text->SetVisible(visible);
    m_up_arrow->SetVisible(visible);
    m_down_arrow->SetVisible(visible);
    m_enter_text->SetVisible(visible);
    m_quit_text->SetVisible(visible);
}

void FlyUI::SetTargetFloor(int floor) {
    m_floor_text->SetNumber(floor);
    m_floor_text->UpdateDisplayText();
    UpdateArrows(floor);
}

void FlyUI::UpdateArrows(int currentFloor) {
    // Up arrow
    if (currentFloor < AppUtil::TOTAL_STORY - 1) {
        m_up_arrow->SetDrawable(m_up_white);
    } else {
        m_up_arrow->SetDrawable(m_up_gray);
    }

    // Down arrow
    if (currentFloor > 0) {
        m_down_arrow->SetDrawable(m_down_white);
    } else {
        m_down_arrow->SetDrawable(m_down_gray);
    }
}

void FlyUI::AddToRoot(Util::Renderer& root) {
    root.AddChild(m_background);
    root.AddChild(m_floor_text);
    root.AddChild(m_up_arrow);
    root.AddChild(m_down_arrow);
    root.AddChild(m_enter_text);
    root.AddChild(m_quit_text);
}
