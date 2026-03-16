#ifndef FLY_UI_HPP
#define FLY_UI_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Renderer.hpp"
#include "NumericDisplayText.hpp"
#include <memory>
#include <vector>

class FlyUI {
public:
    FlyUI();
    void SetVisible(bool visible);
    void SetTargetFloor(int floor);
    void AddToRoot(Util::Renderer& root);

private:
    void UpdateArrows(int currentFloor);

private:
    std::shared_ptr<Util::GameObject> m_background;
    std::shared_ptr<NumericDisplayText> m_floor_text;
    std::shared_ptr<NumericDisplayText> m_enter_text;
    std::shared_ptr<NumericDisplayText> m_quit_text;
    
    std::shared_ptr<Util::GameObject> m_up_arrow;
    std::shared_ptr<Util::GameObject> m_down_arrow;
    
    std::shared_ptr<Util::Image> m_up_white;
    std::shared_ptr<Util::Image> m_up_gray;
    std::shared_ptr<Util::Image> m_down_white;
    std::shared_ptr<Util::Image> m_down_gray;
};

#endif // FLY_UI_HPP
