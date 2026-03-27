#include "UI/MenuUI.hpp"
#include "Core/AppUtil.hpp"

MenuUI::MenuUI() {
    // 3. Item Notice Initialization
    m_item_notice_bg = std::make_shared<Util::GameObject>();
    m_item_notice_bg->SetDrawable(std::make_shared<Util::Image>(AppUtil::GetStaticResourcePath("bmp/Item/itemDialog.bmp")));
    m_item_notice_bg->m_Transform.translation = {141.0f, 0.0f};
    m_item_notice_bg->m_Transform.scale = {0.735f, 0.735f};
    m_item_notice_bg->SetZIndex(90.0f);

    InitText(m_item_notice_text, "", 141.0f, 0.0f, 24);
    m_item_notice_text->SetShowNumber(false);
    m_item_notice_text->SetZIndex(91.0f);

    InitText(m_item_confirm_text, "-Space-", 470.0f, 0.0f, 20); // Far right, bottom-ish
    m_item_confirm_text->SetShowNumber(false);
    m_item_confirm_text->UpdateDisplayText();
    m_item_confirm_text->SetZIndex(91.0f);

    SetVisible(false);
}

void MenuUI::InitText(std::shared_ptr<NumericDisplayText>& text, const std::string& prefix, float x, float y, int size) {
    text = std::make_shared<NumericDisplayText>(AppUtil::GetStaticResourcePath("Font/Cubic_11.ttf"), size);
    text->SetPrefix(prefix);
    text->SetNumber(0);
    text->SetColor(Util::Color::FromRGB(255, 255, 255));
    text->m_Transform.translation = {x, y};
    text->UpdateDisplayText();
}

void MenuUI::SetVisible(bool visible, MenuType type) {
    // Hide all first
    m_item_notice_bg->SetVisible(false);
    m_item_notice_text->SetVisible(false);
    m_item_confirm_text->SetVisible(false);

    if (visible) {
        m_current_menu = type;
        if (type == MenuType::ITEM_NOTICE) {
            m_item_notice_bg->SetVisible(true);
            m_item_notice_text->SetVisible(true);
            m_item_confirm_text->SetVisible(true);
        }
    } else {
        m_current_menu = MenuType::NONE;
    }
}

void MenuUI::SetItemNotice(const std::string& text) {
    m_item_notice_text->SetPrefix(text);
    m_item_notice_text->UpdateDisplayText();
    SetVisible(true, MenuType::ITEM_NOTICE);
}

void MenuUI::AddToRoot(Util::Renderer& root) {
    root.AddChild(m_item_notice_bg);
    root.AddChild(m_item_notice_text);
    root.AddChild(m_item_confirm_text);
}
