#include "UI/NoticeUI.hpp"
#include "Core/AppUtil.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

NoticeUI::NoticeUI() {
    m_notice_bg = std::make_shared<Util::GameObject>();
    m_notice_bg->SetDrawable(std::make_shared<Util::Image>(AppUtil::GetStaticResourcePath("bmp/Special/notice.bmp")));
    m_notice_bg->m_Transform.translation = {141.0f, 0.0f};
    m_notice_bg->m_Transform.scale = {0.735f, 0.735f};
    m_notice_bg->SetZIndex(90.0f);

    auto fontPath = AppUtil::GetStaticResourcePath("Font/Cubic_11.ttf");
    m_close_hint = std::make_shared<NumericDisplayText>(fontPath, 20);
    m_close_hint->SetPrefix("- Press L to Close -");
    m_close_hint->SetShowNumber(false);
    m_close_hint->m_Transform.translation = {141.0f, -250.0f};
    m_close_hint->UpdateDisplayText();
    m_close_hint->SetZIndex(91.0f);

    SetVisible(false);
}

void NoticeUI::run() {
    if (!m_visible) return;

    // Blinking Hint Logic
    m_blink_timer += Util::Time::GetDeltaTimeMs();
    if (m_blink_timer > 1000.0f) m_blink_timer -= 1000.0f;
    if (m_close_hint) m_close_hint->SetVisible(m_blink_timer < 500.0f);
}

void NoticeUI::SetVisible(bool visible) {
    m_visible = visible;
    if (m_notice_bg) m_notice_bg->SetVisible(visible);
    if (m_close_hint) m_close_hint->SetVisible(visible);

    if (visible) {
        m_blink_timer = 0.0f;
    }
}

void NoticeUI::AddToRoot(Util::Renderer& root) {
    if (m_notice_bg) root.AddChild(m_notice_bg);
    if (m_close_hint) root.AddChild(m_close_hint);
}
