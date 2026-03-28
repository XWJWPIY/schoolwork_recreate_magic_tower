#ifndef ENEMY_BOOK_UI_HPP
#define ENEMY_BOOK_UI_HPP

#include "UI/UIComponent.hpp"
#include "UI/NumericDisplayText.hpp"
#include "Core/FloorMap.hpp"
#include "Objects/Player.hpp"
#include <vector>
#include <memory>
#include <string>

class EnemyBookUI : public UIComponent {
public:
    EnemyBookUI(const std::shared_ptr<Player>& player, 
                const std::shared_ptr<FloorMap>& thingsMap);
    
    virtual void run() override;
    virtual void SetVisible(bool visible) override;
    virtual void AddToRoot(Util::Renderer& root) override;
    virtual bool IsIntercepting() const override { return IsActive(); }

    void Refresh();

private:
    void UpdateEnemyList();
    void UpdatePage(int pageIdx);

    std::shared_ptr<Player> m_player;
    std::shared_ptr<FloorMap> m_things_map;
    
    std::shared_ptr<Util::GameObject> m_background;
    std::shared_ptr<NumericDisplayText> m_space_hint;
    std::shared_ptr<NumericDisplayText> m_no_enemy_text;
    
    std::shared_ptr<Util::GameObject> m_left_arrow;
    std::shared_ptr<Util::GameObject> m_right_arrow;

    // UI Slots: 3 entries per page
    std::vector<std::shared_ptr<Util::GameObject>> m_frames;
    std::vector<std::shared_ptr<Util::GameObject>> m_icons;
    
    std::vector<std::shared_ptr<NumericDisplayText>> m_names;
    std::vector<std::shared_ptr<NumericDisplayText>> m_specials;
    std::vector<std::shared_ptr<NumericDisplayText>> m_hps;
    std::vector<std::shared_ptr<NumericDisplayText>> m_atks;
    std::vector<std::shared_ptr<NumericDisplayText>> m_defs;
    std::vector<std::shared_ptr<NumericDisplayText>> m_agis;
    std::vector<std::shared_ptr<NumericDisplayText>> m_atkTimes;
    std::vector<std::shared_ptr<NumericDisplayText>> m_damages;
    std::vector<std::shared_ptr<NumericDisplayText>> m_exps;
    std::vector<std::shared_ptr<NumericDisplayText>> m_coins;

    std::vector<int> m_unique_enemy_ids;
    int m_current_page = 0;
    int m_total_pages = 0;
    
    const int ENTRIES_PER_PAGE = 3;
    float m_blink_timer = 0.0f;
};

#endif // ENEMY_BOOK_UI_HPP
