#ifndef STATUS_UI_HPP
#define STATUS_UI_HPP

#include "UI/NumericDisplayText.hpp"
#include "Util/Renderer.hpp"
#include "UI/UIComponent.hpp"
#include "Core/AppUtil.hpp"
#include <memory>
#include <vector>

class Player;
class FloorMap;

class StatusUI : public UIComponent {
public:
    StatusUI(const std::shared_ptr<Player>& player, 
             const std::shared_ptr<FloorMap>& floorMap,
             unsigned int fontSize = 36);
    virtual ~StatusUI() = default;

    // UIComponent Interface
    void run() override;
    void SetVisible(bool visible) override;
    void AddToRoot(Util::Renderer& root) override;
    bool IsActive() const override { return m_visible; }
    bool IsIntercepting() const override { return false; } // Never blocks movement

    void Update(const std::shared_ptr<Player>& player, int floorNum);

private:
    struct StatEntry {
        AppUtil::Effect effect;
        std::shared_ptr<NumericDisplayText> text;
    };

    std::vector<StatEntry> m_stat_entries;

    // Special fields (non-player-attribute)
    std::shared_ptr<NumericDisplayText> m_floor_text;
    std::shared_ptr<NumericDisplayText> m_manual_hint_text;
    
    // Status and Avatar
    std::shared_ptr<Util::GameObject> m_player_icon;
    std::shared_ptr<NumericDisplayText> m_status_text;
    
    unsigned int m_default_font_size;

    std::shared_ptr<Player> m_player;
    std::shared_ptr<FloorMap> m_road_map;

    // Helper to initialize text objects
    std::shared_ptr<NumericDisplayText> MakeText(float x, float y, const Util::Color& color, unsigned int size);
};

#endif // STATUS_UI_HPP
