#ifndef END_SCENE_UI_HPP
#define END_SCENE_UI_HPP

#include "UI/UIComponent.hpp"
#include "UI/NumericDisplayText.hpp"
#include "Util/GameObject.hpp"
#include <memory>
#include <string>

class EndSceneUI : public UIComponent {
public:
    EndSceneUI(const std::string& fontPath);
    virtual ~EndSceneUI() = default;

    void Show(bool win);
    
    void AddToRoot(Util::Renderer& root) override;
    void SetVisible(bool visible) override;
    void run() override;

    bool CanRestart() const { return m_visible && m_blink_timer > 1000.0f; } // 1s delay

    bool IsActive() const override { return m_visible; }
    bool IsIntercepting() const override { return m_visible; }

private:
    bool m_visible = false;
    bool m_is_win = false;
    float m_blink_timer = 0.0f;

    std::shared_ptr<Util::GameObject> m_background;
    std::shared_ptr<NumericDisplayText> m_status_text;
    std::shared_ptr<NumericDisplayText> m_restart_hint;
};

#endif // END_SCENE_UI_HPP
