#ifndef MENU_UI_HPP
#define MENU_UI_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Renderer.hpp"
#include "UI/NumericDisplayText.hpp"
#include "Core/AppUtil.hpp"
#include <memory>
#include <vector>

class MenuUI {
public:
    enum class MenuType {
        NONE,
        ITEM_NOTICE
    };

    MenuUI();
    virtual ~MenuUI() = default;

    void SetVisible(bool visible, MenuType type = MenuType::NONE);
    void AddToRoot(Util::Renderer& root);

    // Item Notice specific
    void SetItemNotice(const std::string& text);

private:
    void InitText(std::shared_ptr<NumericDisplayText>& text, const std::string& prefix, float x, float y, int size);

private:
    MenuType m_current_menu = MenuType::NONE;

    // Item Notice Overlay
    std::shared_ptr<Util::GameObject> m_item_notice_bg;
    std::shared_ptr<NumericDisplayText> m_item_notice_text;
    std::shared_ptr<NumericDisplayText> m_item_confirm_text;
};

#endif // MENU_UI_HPP
