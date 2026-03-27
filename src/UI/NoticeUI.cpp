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
    SetVisible(false);
}

void NoticeUI::run() {
    if (!m_visible) return;

    // Original logic: Press L or ESCAPE to close
    if (Util::Input::IsKeyDown(Util::Keycode::L) || 
        Util::Input::IsKeyDown(Util::Keycode::ESCAPE)) {
        SetVisible(false);
    }
}

void NoticeUI::SetVisible(bool visible) {
    m_visible = visible;
    m_notice_bg->SetVisible(visible);
}

void NoticeUI::AddToRoot(Util::Renderer& root) {
    root.AddChild(m_notice_bg);
}
