#include "UI/FlyUI.hpp"
#include "Core/AppUtil.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

FlyUI::FlyUI() {
    // Assets from original MenuUI
    m_fly_bg = std::make_shared<Util::GameObject>();
    m_fly_bg->SetDrawable(std::make_shared<Util::Image>(AppUtil::GetStaticResourcePath("bmp/Special/Fly.bmp")));
    m_fly_bg->m_Transform.translation = {141.0f, 0.0f};
    m_fly_bg->m_Transform.scale = {0.735f, 0.735f};
    m_fly_bg->SetZIndex(90.0f);

    auto fontPath = AppUtil::GetStaticResourcePath("Font/Cubic_11.ttf");
    
    // Floor Number
    m_floor_text = std::make_shared<NumericDisplayText>(fontPath, 81);
    m_floor_text->SetZIndex(91.0f);
    m_floor_text->m_Transform.translation = {141.0f, 0.0f};

    // Hints
    auto initHint = [&](std::shared_ptr<NumericDisplayText>& text, const std::string& prefix, float x, float y) {
        text = std::make_shared<NumericDisplayText>(fontPath, 24);
        text->SetPrefix(prefix);
        text->SetShowNumber(false);
        text->m_Transform.translation = {x, y};
        text->UpdateDisplayText();
        text->SetZIndex(92.0f);
    };
    initHint(m_enter_text, "-Enter-", 300.0f, -110.0f);
    initHint(m_quit_text, "-Quit-(F)", 350.0f, -250.0f);

    // Arrows
    m_up_white = std::make_shared<Util::Image>(AppUtil::GetStaticResourcePath("bmp/Special/up_arrow_white.png"));
    m_up_gray = std::make_shared<Util::Image>(AppUtil::GetStaticResourcePath("bmp/Special/up_arrow_gray.png"));
    m_down_white = std::make_shared<Util::Image>(AppUtil::GetStaticResourcePath("bmp/Special/down_arrow_white.png"));
    m_down_gray = std::make_shared<Util::Image>(AppUtil::GetStaticResourcePath("bmp/Special/down_arrow_gray.png"));

    m_up_arrow = std::make_shared<Util::GameObject>();
    m_up_arrow->m_Transform.translation = {141.0f, 220.0f};
    m_up_arrow->m_Transform.scale = {0.5f, 0.5f};
    m_up_arrow->SetZIndex(92.0f);
    
    m_down_arrow = std::make_shared<Util::GameObject>();
    m_down_arrow->m_Transform.translation = {141.0f, -220.0f};
    m_down_arrow->m_Transform.scale = {0.5f, 0.5f};
    m_down_arrow->SetZIndex(92.0f);

    SetVisible(false);
}

void FlyUI::Start(int currentStory, TeleportCallback onTeleport) {
    m_current_story = currentStory;
    m_preview_floor = currentStory;
    m_on_teleport = std::move(onTeleport);
    SetTargetFloor(m_preview_floor);
    SetVisible(true);
}

void FlyUI::run() {
    if (!m_visible) return;

    if (Util::Input::IsKeyDown(Util::Keycode::W) || Util::Input::IsKeyDown(Util::Keycode::UP)) {
        if (m_preview_floor < AppUtil::TOTAL_STORY - 1) {
            m_preview_floor++;
            SetTargetFloor(m_preview_floor);
        }
    } else if (Util::Input::IsKeyDown(Util::Keycode::S) || Util::Input::IsKeyDown(Util::Keycode::DOWN)) {
        if (m_preview_floor > 0) {
            m_preview_floor--;
            SetTargetFloor(m_preview_floor);
        }
    }

    if (Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
        if (m_on_teleport) m_on_teleport(m_preview_floor, 0);
        SetVisible(false);
    }
    
    if (Util::Input::IsKeyDown(Util::Keycode::ESCAPE) || Util::Input::IsKeyDown(Util::Keycode::F)) {
        SetVisible(false);
    }

    if (!m_visible) return;

    // Blinking Hint Logic
    m_blink_timer += Util::Time::GetDeltaTimeMs();
    if (m_blink_timer > 1000.0f) m_blink_timer -= 1000.0f;
    if (m_enter_text) m_enter_text->SetVisible(m_blink_timer < 500.0f);
}

void FlyUI::SetVisible(bool visible) {
    m_visible = visible;
    if (m_fly_bg) m_fly_bg->SetVisible(visible);
    if (m_floor_text) m_floor_text->SetVisible(visible);
    if (m_enter_text) m_enter_text->SetVisible(visible);
    if (m_quit_text) m_quit_text->SetVisible(visible);
    if (m_up_arrow) m_up_arrow->SetVisible(visible);
    if (m_down_arrow) m_down_arrow->SetVisible(visible);

    if (visible) {
        m_blink_timer = 0.0f;
    }
}

void FlyUI::SetTargetFloor(int floor) {
    m_floor_text->SetNumber(floor);
    m_floor_text->UpdateDisplayText();
    UpdateArrows(floor);
}

void FlyUI::UpdateArrows(int currentFloor) {
    if (currentFloor < AppUtil::TOTAL_STORY - 1) m_up_arrow->SetDrawable(m_up_white);
    else m_up_arrow->SetDrawable(m_up_gray);

    if (currentFloor > 0) m_down_arrow->SetDrawable(m_down_white);
    else m_down_arrow->SetDrawable(m_down_gray);
}

void FlyUI::AddToRoot(Util::Renderer& root) {
    if (m_fly_bg) root.AddChild(m_fly_bg);
    if (m_floor_text) root.AddChild(m_floor_text);
    if (m_enter_text) root.AddChild(m_enter_text);
    if (m_quit_text) root.AddChild(m_quit_text);
    if (m_up_arrow) root.AddChild(m_up_arrow);
    if (m_down_arrow) root.AddChild(m_down_arrow);
}
