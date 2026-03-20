#include "MenuUI.hpp"

MenuUI::MenuUI() {
    // 1. Notice Initialization
    m_notice_bg = std::make_shared<Util::GameObject>();
    m_notice_bg->SetDrawable(std::make_shared<Util::Image>(MAGIC_TOWER_RESOURCE_DIR "/bmp/Special/notice.bmp"));
    m_notice_bg->m_Transform.translation = {141.0f, 0.0f};
    m_notice_bg->m_Transform.scale = {0.735f, 0.735f};
    m_notice_bg->SetZIndex(90.0f);

    // 2. Fly Initialization
    m_fly_bg = std::make_shared<Util::GameObject>();
    m_fly_bg->SetDrawable(std::make_shared<Util::Image>(MAGIC_TOWER_RESOURCE_DIR "/bmp/Special/Fly.bmp"));
    m_fly_bg->m_Transform.translation = {141.0f, 0.0f};
    m_fly_bg->m_Transform.scale = {0.735f, 0.735f};
    m_fly_bg->SetZIndex(90.0f);

    InitText(m_floor_text, "", 141.0f, 0.0f, 81);
    m_floor_text->SetZIndex(91.0f);

    InitText(m_enter_text, "-Enter-", 300.0f, -110.0f, 24);
    m_enter_text->SetShowNumber(false);
    m_enter_text->UpdateDisplayText();
    m_enter_text->SetZIndex(92.0f);

    InitText(m_quit_text, "-Quit-(F)", 350.0f, -250.0f, 24);
    m_quit_text->SetShowNumber(false);
    m_quit_text->UpdateDisplayText();
    m_quit_text->SetZIndex(92.0f);

    // Arrows
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

    // 3. Item Notice Initialization
    m_item_notice_bg = std::make_shared<Util::GameObject>();
    m_item_notice_bg->SetDrawable(std::make_shared<Util::Image>(MAGIC_TOWER_RESOURCE_DIR "/bmp/Item/itemDialog.bmp"));
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

    // 4. Shop Initialization
    m_shop_bg = std::make_shared<Util::GameObject>();
    m_shop_bg->SetDrawable(std::make_shared<Util::Image>(MAGIC_TOWER_RESOURCE_DIR "/bmp/Shop/ShopDialog.bmp"));
    m_shop_bg->m_Transform.translation = {141.0f, 0.0f};
    m_shop_bg->m_Transform.scale = {0.735f, 0.735f};
    m_shop_bg->SetZIndex(90.0f);

    m_shop_icon = std::make_shared<Util::GameObject>();
    m_shop_icon->m_Transform.translation = { 28.0f, 141.0f }; // Centered in the top-left box
    m_shop_icon->m_Transform.scale = { 0.735f, 0.735f };      // Match UI scale
    m_shop_icon->SetZIndex(91.0f);

    InitText(m_shop_title, "Store Explorer", 141.0f, 195.0f, 32);
    m_shop_title->SetShowNumber(false);
    m_shop_title->UpdateDisplayText();
    m_shop_title->SetZIndex(91.0f);

    for (int i = 0; i < 4; ++i) {
        auto prompt = std::make_shared<NumericDisplayText>(MAGIC_TOWER_RESOURCE_DIR "/Font/Cubic_11.ttf", 20);
        prompt->SetShowNumber(false);
        if (i == 3) {
            prompt->m_Transform.translation = {160.0f, 140.0f - 1 * 28.0f}; // 例如調成 85.0f
        } else {
            prompt->m_Transform.translation = {190.0f, 140.0f - i * 28.0f};
        }
        prompt->SetZIndex(91.0f);
        m_shop_prompts.push_back(prompt);
    }

    for (int i = 0; i < 4; ++i) { // 3 items + 1 exit
        auto opt = std::make_shared<NumericDisplayText>(MAGIC_TOWER_RESOURCE_DIR "/Font/Cubic_11.ttf", 24);
        opt->SetShowNumber(false);
        opt->m_Transform.translation = {150.0f, -30.0f - i * 48.0f};
        opt->SetZIndex(91.0f);
        m_shop_options.push_back(opt);
    }

    m_shop_selector = std::make_shared<Util::GameObject>();
    m_shop_selector->SetDrawable(std::make_shared<Util::Image>(MAGIC_TOWER_RESOURCE_DIR "/bmp/Special/right_arrow_white.png"));
    m_shop_selector->m_Transform.scale = {0.5f, 0.5f};
    m_shop_selector->SetZIndex(92.0f);

    SetVisible(false);
}

void MenuUI::InitText(std::shared_ptr<NumericDisplayText>& text, const std::string& prefix, float x, float y, int size) {
    text = std::make_shared<NumericDisplayText>(MAGIC_TOWER_RESOURCE_DIR "/Font/Cubic_11.ttf", size);
    text->SetPrefix(prefix);
    text->SetNumber(0);
    text->SetColor(Util::Color::FromRGB(255, 255, 255));
    text->m_Transform.translation = {x, y};
    text->UpdateDisplayText();
}

void MenuUI::SetVisible(bool visible, MenuType type) {
    // Hide all first
    m_notice_bg->SetVisible(false);
    m_fly_bg->SetVisible(false);
    m_floor_text->SetVisible(false);
    m_enter_text->SetVisible(false);
    m_quit_text->SetVisible(false);
    m_up_arrow->SetVisible(false);
    m_down_arrow->SetVisible(false);
    m_item_notice_bg->SetVisible(false);
    m_item_notice_text->SetVisible(false);
    m_item_confirm_text->SetVisible(false);
    m_shop_bg->SetVisible(false);
    m_shop_icon->SetVisible(false);
    m_shop_title->SetVisible(false);
    for (auto& prompt : m_shop_prompts) prompt->SetVisible(false);
    for (auto& opt : m_shop_options) opt->SetVisible(false);
    m_shop_selector->SetVisible(false);

    if (!visible) {
        m_current_menu = MenuType::NONE;
        return;
    }

    m_current_menu = type;
    if (type == MenuType::NOTICE) {
        m_notice_bg->SetVisible(true);
    } else if (type == MenuType::FAST_ELEVATOR) {
        m_fly_bg->SetVisible(true);
        m_floor_text->SetVisible(true);
        m_enter_text->SetVisible(true);
        m_quit_text->SetVisible(true);
        m_up_arrow->SetVisible(true);
        m_down_arrow->SetVisible(true);
    } else if (type == MenuType::ITEM_NOTICE) {
        m_item_notice_bg->SetVisible(true);
        m_item_notice_text->SetVisible(true);
        m_item_confirm_text->SetVisible(true);
    } else if (type == MenuType::SHOP) {
        m_shop_bg->SetVisible(true);
        m_shop_icon->SetVisible(true);
        m_shop_title->SetVisible(true);
        for (auto& prompt : m_shop_prompts) prompt->SetVisible(true);
        for (auto& opt : m_shop_options) opt->SetVisible(true);
        m_shop_selector->SetVisible(true);
    }
}

void MenuUI::SetTargetFloor(int floor) {
    m_floor_text->SetNumber(floor);
    m_floor_text->UpdateDisplayText();
    UpdateArrows(floor);
}

void MenuUI::SetItemNotice(const std::string& text) {
    m_item_notice_text->SetPrefix(text);
    m_item_notice_text->UpdateDisplayText();
    SetVisible(true, MenuType::ITEM_NOTICE);
}

void MenuUI::UpdateArrows(int currentFloor) {
    if (currentFloor < AppUtil::TOTAL_STORY - 1) m_up_arrow->SetDrawable(m_up_white);
    else m_up_arrow->SetDrawable(m_up_gray);

    if (currentFloor > 0) m_down_arrow->SetDrawable(m_down_white);
    else m_down_arrow->SetDrawable(m_down_gray);
}

void MenuUI::AddToRoot(Util::Renderer& root) {
    root.AddChild(m_notice_bg);
    root.AddChild(m_fly_bg);
    root.AddChild(m_floor_text);
    root.AddChild(m_enter_text);
    root.AddChild(m_quit_text);
    root.AddChild(m_up_arrow);
    root.AddChild(m_down_arrow);
    root.AddChild(m_item_notice_bg);
    root.AddChild(m_item_notice_text);
    root.AddChild(m_item_confirm_text);

    root.AddChild(m_shop_bg);
    root.AddChild(m_shop_icon);
    root.AddChild(m_shop_title);
    for (auto& prompt : m_shop_prompts) root.AddChild(prompt);
    for (auto& opt : m_shop_options) root.AddChild(opt);
    root.AddChild(m_shop_selector);
}

void MenuUI::SetShopData(const AppUtil::ShopData& data) {
    if (!data.icon_path.empty()) {
        m_shop_icon->SetDrawable(std::make_shared<Util::Image>(MAGIC_TOWER_RESOURCE_DIR "/bmp/Shop/" + data.icon_path));
    }
    m_shop_title->SetPrefix(data.title);
    m_shop_title->UpdateDisplayText();
    
    for (size_t i = 0; i < m_shop_prompts.size(); ++i) {
        if (i < data.prompts.size() && !data.prompts[i].empty()) {
            m_shop_prompts[i]->SetPrefix(data.prompts[i]);
            m_shop_prompts[i]->SetVisible(true);
            m_shop_prompts[i]->UpdateDisplayText();
        } else {
            m_shop_prompts[i]->SetVisible(false);
        }
    }
    for (size_t i = 0; i < m_shop_options.size(); ++i) {
        if (i < data.options.size()) {
            m_shop_options[i]->SetPrefix(data.options[i].text);
            m_shop_options[i]->SetVisible(true);
        } else {
            m_shop_options[i]->SetPrefix("");
            m_shop_options[i]->SetVisible(false);
        }
        m_shop_options[i]->UpdateDisplayText();
    }
    // UpdateShopSelection(0); // Removed to allow persistence across purchases
}

void MenuUI::UpdateShopSelection(int index) {
    if (index >= 0 && index < static_cast<int>(m_shop_options.size())) {
        m_shop_selector->m_Transform.translation = m_shop_options[index]->m_Transform.translation + glm::vec2(-150.0f, 0.0f);
    }
}
