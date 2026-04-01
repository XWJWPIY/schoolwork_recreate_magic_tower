#include "UI/EndSceneUI.hpp"
#include "Core/AppUtil.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"

EndSceneUI::EndSceneUI(const std::string& fontPath) {
    m_background = std::make_shared<Util::GameObject>(
        std::make_unique<Util::Image>(AppUtil::GetStaticResourcePath("bmp/Scene/end.BMP")), 50.0f);
    m_background->m_Transform.translation = {0.0f, 0.0f};
    m_background->m_Transform.scale = {1.0f, 1.0f};

    m_status_text = std::make_shared<NumericDisplayText>(fontPath, 64);
    m_status_text->SetZIndex(51.0f);
    m_status_text->SetAlignment(NumericDisplayText::Align::CENTER);
    m_status_text->m_Transform.translation = {0.0f, 50.0f};
    m_status_text->SetColor(Util::Color::FromRGB(255, 255, 255));
    m_status_text->SetShowNumber(false);

    m_restart_hint = std::make_shared<NumericDisplayText>(fontPath, 24);
    m_restart_hint->SetZIndex(51.0f);
    m_restart_hint->SetAlignment(NumericDisplayText::Align::CENTER);
    m_restart_hint->m_Transform.translation = {0.0f, -150.0f};
    m_restart_hint->SetColor(Util::Color::FromRGB(200, 200, 200));
    m_restart_hint->SetShowNumber(false);
    m_restart_hint->SetPrefix("Press space to restart");
    m_restart_hint->UpdateDisplayText();

    SetVisible(false);
}

void EndSceneUI::Show(bool win) {
    m_is_win = win;
    m_status_text->SetPrefix(win ? "You Win!!" : "Game Over");
    m_status_text->UpdateDisplayText();
    m_blink_timer = 0.0f; // Reset timer for delay
    SetVisible(true);
}

void EndSceneUI::AddToRoot(Util::Renderer& root) {
    if (m_background) root.AddChild(m_background);
    if (m_status_text) root.AddChild(m_status_text);
    if (m_restart_hint) root.AddChild(m_restart_hint);
}

void EndSceneUI::SetVisible(bool visible) {
    m_visible = visible;
    if (m_background) m_background->SetVisible(visible);
    if (m_status_text) m_status_text->SetVisible(visible);
    if (m_restart_hint) m_restart_hint->SetVisible(visible);
}

void EndSceneUI::run() {
    if (!m_visible) return;

    m_blink_timer += Util::Time::GetDeltaTimeMs();
    
    bool canRestart = m_blink_timer > 1000.0f;
    if (m_restart_hint) {
        if (!canRestart) {
            m_restart_hint->SetVisible(false);
        } else {
            // Blinking effect
            float blinkTimer = m_blink_timer - 1000.0f;
            if (static_cast<int>(blinkTimer) % 1000 < 500) {
                m_restart_hint->SetVisible(true);
            } else {
                m_restart_hint->SetVisible(false);
            }
        }
    }
}
