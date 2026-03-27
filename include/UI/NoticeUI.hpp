#ifndef NOTICE_UI_HPP
#define NOTICE_UI_HPP

#include "UI/UIComponent.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <memory>

class NoticeUI : public UIComponent {
public:
    NoticeUI();
    virtual ~NoticeUI() = default;

    // UIComponent Interface
    void run() override; // Handles L/ESCAPE to close
    void SetVisible(bool visible) override;
    void AddToRoot(Util::Renderer& root) override;
    bool IsActive() const override { return m_visible; }
    bool IsIntercepting() const override { return IsActive(); }

private:
    bool m_visible = false;
    std::shared_ptr<Util::GameObject> m_notice_bg;
};

#endif // NOTICE_UI_HPP
