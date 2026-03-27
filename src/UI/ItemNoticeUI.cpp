#include "UI/ItemNoticeUI.hpp"
#include "Core/AppUtil.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Image.hpp"

ItemNoticeUI::ItemNoticeUI() {
    m_item_notice_bg = std::make_shared<Util::GameObject>();
    m_item_notice_bg->SetDrawable(std::make_shared<Util::Image>(AppUtil::GetStaticResourcePath("bmp/Item/itemDialog.bmp")));
    m_item_notice_bg->m_Transform.translation = {141.0f, 0.0f};
    m_item_notice_bg->m_Transform.scale = {0.735f, 0.735f};
    m_item_notice_bg->SetZIndex(90.0f);

    auto fontPath = AppUtil::GetStaticResourcePath("Font/Cubic_11.ttf");
    
    m_item_notice_text = std::make_shared<NumericDisplayText>(fontPath, 24);
    m_item_notice_text->SetShowNumber(false);
    m_item_notice_text->m_Transform.translation = {141.0f, 0.0f};
    m_item_notice_text->SetZIndex(91.0f);

    m_item_confirm_text = std::make_shared<NumericDisplayText>(fontPath, 20);
    m_item_confirm_text->SetPrefix("-Space-");
    m_item_confirm_text->SetShowNumber(false);
    m_item_confirm_text->m_Transform.translation = {470.0f, 0.0f};
    m_item_confirm_text->UpdateDisplayText();
    m_item_confirm_text->SetZIndex(91.0f);

    SetVisible(false);
}

void ItemNoticeUI::run() {
    if (!m_visible) return;

    if (Util::Input::IsKeyDown(Util::Keycode::SPACE) || 
        Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
        SetVisible(false);
    }
}

void ItemNoticeUI::Show(const std::string& text) {
    m_item_notice_text->SetPrefix(text);
    m_item_notice_text->UpdateDisplayText();
    SetVisible(true);
}

void ItemNoticeUI::SetVisible(bool visible) {
    m_visible = visible;
    m_item_notice_bg->SetVisible(visible);
    m_item_notice_text->SetVisible(visible);
    m_item_confirm_text->SetVisible(visible);
}

void ItemNoticeUI::AddToRoot(Util::Renderer& root) {
    root.AddChild(m_item_notice_bg);
    root.AddChild(m_item_notice_text);
    root.AddChild(m_item_confirm_text);
}
