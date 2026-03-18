#ifndef MENU_UI_HPP
#define MENU_UI_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Renderer.hpp"
#include "NumericDisplayText.hpp"
#include "AppUtil.hpp"
#include <memory>
#include <vector>

class MenuUI {
public:
    enum class MenuType {
        NONE,
        NOTICE,
        FAST_ELEVATOR,
        ITEM_NOTICE
        // FUTURE: MONSTER_MANUAL
    };

    MenuUI();
    void SetVisible(bool visible, MenuType type = MenuType::NONE);
    void AddToRoot(Util::Renderer& root);

    // Fast Elevator specific
    void SetTargetFloor(int floor);

    // Item Notice specific
    void SetItemNotice(const std::string& text);

private:
    void UpdateArrows(int currentFloor);
    void InitText(std::shared_ptr<NumericDisplayText>& text, const std::string& prefix, float x, float y, int size);

private:
    MenuType m_current_menu = MenuType::NONE;

    // Notice Overlay
    std::shared_ptr<Util::GameObject> m_notice_bg;

    // Fly Overlay
    std::shared_ptr<Util::GameObject> m_fly_bg;
    std::shared_ptr<NumericDisplayText> m_floor_text;
    std::shared_ptr<NumericDisplayText> m_enter_text;
    std::shared_ptr<NumericDisplayText> m_quit_text;
    
    std::shared_ptr<Util::GameObject> m_up_arrow;
    std::shared_ptr<Util::GameObject> m_down_arrow;
    
    std::shared_ptr<Util::Image> m_up_white;
    std::shared_ptr<Util::Image> m_up_gray;
    std::shared_ptr<Util::Image> m_down_white;
    std::shared_ptr<Util::Image> m_down_gray;

    // Item Notice Overlay
    std::shared_ptr<Util::GameObject> m_item_notice_bg;
    std::shared_ptr<NumericDisplayText> m_item_notice_text;
    std::shared_ptr<NumericDisplayText> m_item_confirm_text;
};

#endif // MENU_UI_HPP
