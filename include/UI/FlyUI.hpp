#ifndef FLY_UI_HPP
#define FLY_UI_HPP

#include "UI/UIComponent.hpp"
#include "UI/NumericDisplayText.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <functional>
#include <memory>

class Player;

class FlyUI : public UIComponent {
public:
    using TeleportCallback = std::function<void(int targetStory, int stairId)>;

    FlyUI();
    virtual ~FlyUI() = default;

    // UIComponent Interface
    void run() override;
    void SetVisible(bool visible) override;
    void AddToRoot(Util::Renderer& root) override;
    bool IsActive() const override { return m_visible; }
    bool IsIntercepting() const override { return IsActive(); }

    // Logic
    void SetPlayer(std::shared_ptr<Player> player) { m_player = player; }
    void Start(int currentStory, TeleportCallback onTeleport);
    void SetTargetFloor(int floor);

private:
    void UpdateArrows(int currentFloor);

private:
    std::shared_ptr<Player> m_player;
    int m_current_story = 0;
    int m_preview_floor = 0;
    TeleportCallback m_on_teleport;

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
    
    float m_blink_timer = 0.0f;
};

#endif // FLY_UI_HPP
