#include "ShopUI.hpp"
#include "AppUtil.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Image.hpp"

namespace {
    // Layout Constants
    const glm::vec2 OPTION_START_POS = {141.0f, -10.0f};
    const float OPTION_SPACING_Y = 60.0f;
    const glm::vec2 SELECTOR_OFFSET = {-150.0f, 0.0f};
    const glm::vec2 PRICE_DISPLAY_POS = {155.0f, 130.0f};
    const float SCALE_ARROW = 0.5f;
    const float Z_INDEX_OPTIONS = 15.0f;
    const float Z_INDEX_SELECTOR = 16.0f;
}

ShopUI::ShopUI(const std::string& fontPath) {
    // Initialize Option Lines
    for (int i = 0; i < 4; ++i) { 
        auto opt = std::make_shared<NumericDisplayText>(fontPath, 24);
        opt->SetShowNumber(false);
        opt->m_Transform.translation = {OPTION_START_POS.x, OPTION_START_POS.y - i * OPTION_SPACING_Y}; 
        opt->SetZIndex(Z_INDEX_OPTIONS);
        m_options.push_back(opt);
    }

    // Initialize Selection Arrow
    m_selector = std::make_shared<Util::GameObject>(
        std::make_unique<Util::Image>(AppUtil::GetStaticResourcePath("bmp/Special/right_arrow_white.png")), Z_INDEX_SELECTOR);
    m_selector->m_Transform.scale = {SCALE_ARROW, SCALE_ARROW};

    // Initialize Special Price Display
    m_price_display = std::make_shared<NumericDisplayText>(fontPath, 24);
    m_price_display->SetZIndex(Z_INDEX_OPTIONS);
    m_price_display->SetShowNumber(false);
}

void ShopUI::Start(const AppUtil::ShopData& data, std::function<void(int)> onSelect) {
    m_data = data;
    m_on_select = std::move(onSelect);
    m_selection = 0;
    
    // Refresh then Show to ensure correct initial state
    Refresh(m_data);
    SetVisible(true);
}

void ShopUI::HandleInput() {
    if (!m_visible) return;

    const int opt_count = static_cast<int>(m_data.options.size());
    if (opt_count == 0) return;

    if (Util::Input::IsKeyDown(Util::Keycode::W) || Util::Input::IsKeyDown(Util::Keycode::UP)) {
        m_selection = (m_selection - 1 + opt_count) % opt_count;
        UpdateSelectionVisuals();
    }
    else if (Util::Input::IsKeyDown(Util::Keycode::S) || Util::Input::IsKeyDown(Util::Keycode::DOWN)) {
        m_selection = (m_selection + 1) % opt_count;
        UpdateSelectionVisuals();
    }
    else if (Util::Input::IsKeyDown(Util::Keycode::SPACE) || Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
        if (m_on_select) m_on_select(m_selection);
    }
    else if (Util::Input::IsKeyDown(Util::Keycode::ESCAPE) || Util::Input::IsKeyDown(Util::Keycode::Q)) {
        // Find "Exit" index or fallback to last option
        int exitIdx = opt_count - 1;
        for (int i = 0; i < opt_count; ++i) {
            if (m_data.options[i].text == "Exit") {
                exitIdx = i;
                break;
            }
        }
        if (m_on_select) m_on_select(exitIdx);
    }
}

void ShopUI::Refresh(const AppUtil::ShopData& data) {
    m_data = data;
    
    // Update Option Text and Visibility
    for (size_t i = 0; i < m_options.size(); ++i) {
        bool inRange = (i < m_data.options.size());
        if (inRange) {
            m_options[i]->SetPrefix(m_data.options[i].text);
            m_options[i]->UpdateDisplayText();
        }
        m_options[i]->SetVisible(m_visible && inRange);
    }

    // Update Special Price Display
    if (!m_data.special_price_str.empty()) {
        m_price_display->SetPrefix(m_data.special_price_str);
        m_price_display->UpdateDisplayText();
        m_price_display->m_Transform.translation = PRICE_DISPLAY_POS; 
        m_price_display->SetVisible(m_visible);
    } else {
        m_price_display->SetVisible(false);
    }

    UpdateSelectionVisuals();
}

void ShopUI::UpdateSelectionVisuals() {
    bool validSel = (m_selection >= 0 && m_selection < static_cast<int>(m_options.size()));
    if (validSel && m_selection < static_cast<int>(m_data.options.size())) {
        m_selector->m_Transform.translation = m_options[m_selection]->m_Transform.translation + SELECTOR_OFFSET;
        m_selector->SetVisible(m_visible);
    } else {
        m_selector->SetVisible(false);
    }
}

void ShopUI::SetVisible(bool visible) {
    m_visible = visible;
    
    // Delegate visiblity update to Refresh logic for consistency
    Refresh(m_data);
}

void ShopUI::AddToRoot(Util::Renderer& root) {
    for (auto& opt : m_options) root.AddChild(opt);
    root.AddChild(m_selector);
    root.AddChild(m_price_display);
}
