#ifndef ENEMY_BOOK_UI_HPP
#define ENEMY_BOOK_UI_HPP

#include "UI/UIComponent.hpp"
#include "UI/NumericDisplayText.hpp"
#include "Objects/Player.hpp"
#include "Core/FloorMap.hpp"
#include <vector>
#include <memory>
#include <string>

class EnemyBookUI : public UIComponent {
public:
    EnemyBookUI(const std::shared_ptr<Player>& player, 
                const std::shared_ptr<FloorMap>& thingsMap);

    void run() override;
    bool IsIntercepting() const override { return m_visible; }
    bool IsActive() const override { return m_visible; }
    void SetVisible(bool visible) override;
    void AddToRoot(Util::Renderer& root) override;

    void Refresh();

private:
    struct EnemyEntry {
        std::shared_ptr<Util::GameObject> frame;
        std::shared_ptr<Util::GameObject> icon;
        std::shared_ptr<NumericDisplayText> name;
        std::shared_ptr<NumericDisplayText> special;
        std::vector<std::shared_ptr<NumericDisplayText>> stats; // hp, atk, def, agi, atkTime, damage, exp, gold

        void Initialize(const std::string& fontPath, float baseY, float bgX);
        void SetVisible(bool visible);
        void AddToRoot(Util::Renderer& root);
        void Update(const AppUtil::ObjectMetadata& meta, Player* player);
    };

    void UpdateEnemyList();
    void UpdatePage(int pageIdx);

    static constexpr int ENTRIES_PER_PAGE = 3;
    static constexpr int STAT_COUNT = 8; // hp, atk, def, agi, atkTime, damage, exp, gold

    std::shared_ptr<Player> m_player;
    std::shared_ptr<FloorMap> m_things_map;

    std::shared_ptr<Util::GameObject> m_background;
    std::shared_ptr<NumericDisplayText> m_space_hint;
    std::shared_ptr<NumericDisplayText> m_no_enemy_text;
    std::shared_ptr<Util::GameObject> m_left_arrow;
    std::shared_ptr<Util::GameObject> m_right_arrow;

    std::vector<EnemyEntry> m_entries;
    std::vector<int> m_unique_enemy_ids;
    
    int m_current_page = 0;
    int m_total_pages = 0;
};

#endif // ENEMY_BOOK_UI_HPP
