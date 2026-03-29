#ifndef ITEM_NOTICE_UI_HPP
#define ITEM_NOTICE_UI_HPP

#include "UI/UIComponent.hpp"
#include "UI/NumericDisplayText.hpp"
#include "Util/GameObject.hpp"
#include <memory>
#include <string>

class ItemNoticeUI : public UIComponent {
public:
    ItemNoticeUI();
    virtual ~ItemNoticeUI() = default;

    // UIComponent Interface
    void run() override; // Handles SPACE/RETURN to close
    void SetVisible(bool visible) override;
    void AddToRoot(Util::Renderer& root) override;
    bool IsActive() const override { return m_visible; }
    bool IsIntercepting() const override { return IsActive(); }

    // Specialized Logic
    void Show(const std::string& text);

private:
    std::shared_ptr<Util::GameObject> m_item_notice_bg;
    std::shared_ptr<NumericDisplayText> m_item_notice_text;
    std::shared_ptr<NumericDisplayText> m_item_confirm_text;
    float m_blink_timer = 0.0f;
};

#endif // ITEM_NOTICE_UI_HPP
