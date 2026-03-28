#include "UI/EnemyBookUI.hpp"
#include "Core/AppUtil.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Image.hpp"
#include <algorithm>

namespace {
    const glm::vec2 BG_POS = {141.0f, 0.0f};
    const float ENTRY_START_Y = 150.0f;
    const float ENTRY_SPACING_Y = 150.0f;
    const float ICON_OFFSET_X = -220.0f;

    const float Z_BG = 15.0f;
    const float Z_FRAME = 16.0f;
    const float Z_ICON = 17.0f;
    const float Z_TEXT = 18.0f;
    const float Z_HINT = 20.0f;
    
    // Relative offsets within an entry (Relative to BG_POS + {0, baseY})
    const glm::vec2 OFF_NAME    = {-165.0f, 40.0f};
    const glm::vec2 OFF_SPECIAL = {15.0f, 40.0f};
    
    const glm::vec2 OFF_HP      = {-165.0f, 0.0f}; 
    const glm::vec2 OFF_ATK     = {-70.0f, 0.0f};
    const glm::vec2 OFF_DEF     = {55.0f, 0.0f};
    const glm::vec2 OFF_AGI     = {160.0f, 0.0f};
    
    const glm::vec2 OFF_ATK_TIME = {-165.0f, -40.0f};
    const glm::vec2 OFF_DAMAGE   = {-70.0f, -40.0f};
    const glm::vec2 OFF_EXP      = {55.0f, -40.0f};
    const glm::vec2 OFF_COIN     = {160.0f, -40.0f};

    const Util::Color CLR_GREEN  = Util::Color::FromRGB(0, 255, 0);
    const Util::Color CLR_RED    = Util::Color::FromRGB(255, 0, 0);
    const Util::Color CLR_PINK   = Util::Color::FromRGB(255, 105, 180);
    const Util::Color CLR_ORANGE = Util::Color::FromRGB(255, 165, 0);
    const Util::Color CLR_BLUE   = Util::Color::FromRGB(0, 191, 255);
    const Util::Color CLR_YELLOW = Util::Color::FromRGB(255, 255, 0);
    const Util::Color CLR_WHITE  = Util::Color::FromRGB(255, 255, 255);
}

EnemyBookUI::EnemyBookUI(const std::shared_ptr<Player>& player, 
                         const std::shared_ptr<FloorMap>& thingsMap)
    : m_player(player), m_things_map(thingsMap) {
    
    auto fontPath = AppUtil::GetStaticResourcePath("Font/Cubic_11.ttf");

    m_background = std::make_shared<Util::GameObject>();
    m_background->SetDrawable(std::make_shared<Util::Image>(AppUtil::GetStaticResourcePath("bmp/Special/Black.bmp")));
    m_background->m_Transform.translation = BG_POS;
    m_background->m_Transform.scale = {0.735f, 0.735f};
    m_background->SetZIndex(Z_BG);

    for (int i = 0; i < ENTRIES_PER_PAGE; ++i) {
        float baseY = ENTRY_START_Y - i * ENTRY_SPACING_Y;
        glm::vec2 basePos = {BG_POS.x, baseY};

        auto frame = std::make_shared<Util::GameObject>();
        frame->SetDrawable(std::make_shared<Util::Image>(AppUtil::GetStaticResourcePath("bmp/Special/EnemyFrame.bmp")));
        frame->m_Transform.translation = basePos + glm::vec2(ICON_OFFSET_X, 20.0f);
        frame->SetZIndex(Z_FRAME);
        m_frames.push_back(frame);

        auto icon = std::make_shared<Util::GameObject>();
        icon->m_Transform.translation = frame->m_Transform.translation;
        icon->SetZIndex(Z_ICON);
        m_icons.push_back(icon);

        auto initT = [&](std::vector<std::shared_ptr<NumericDisplayText>>& vec, const std::string& key, const std::string& def, const glm::vec2& off, const Util::Color& clr) {
            auto t = std::make_shared<NumericDisplayText>(fontPath, 22);
            t->SetAlignLeft(true);
            t->SetPrefix(AppUtil::GetGlobalString(key, def)); t->SetColor(clr);
            t->m_Transform.translation = basePos + off;
            t->SetZIndex(Z_TEXT);
            t->UpdateDisplayText();
            vec.push_back(t);
        };

        initT(m_names,    "label_name", "Name: ", OFF_NAME,    CLR_WHITE);
        initT(m_specials, "label_special", "Special: ", OFF_SPECIAL, CLR_RED);
        initT(m_hps,      "label_hp", "HP: ",   OFF_HP,      CLR_GREEN);
        initT(m_atks,     "label_atk", "ATK: ",  OFF_ATK,     CLR_PINK);
        initT(m_defs,     "label_def", "DEF: ",  OFF_DEF,     CLR_ORANGE);
        initT(m_agis,     "label_agi", "AGI: ",  OFF_AGI,     CLR_GREEN);
        initT(m_atkTimes, "label_atk_time", "Times: ", OFF_ATK_TIME,CLR_BLUE);
        initT(m_damages,  "label_damage", "Damage: ", OFF_DAMAGE,  CLR_RED);
        initT(m_exps,     "label_exp", "EXP: ",  OFF_EXP,     CLR_WHITE);
        initT(m_coins,    "label_gold", "GOLD: ", OFF_COIN,    CLR_YELLOW);
    }

    m_space_hint = std::make_shared<NumericDisplayText>(fontPath, 22);
    m_space_hint->SetPrefix(AppUtil::GetGlobalString("hint_press_d", "Press D")); m_space_hint->SetShowNumber(false);
    m_space_hint->m_Transform.translation = {330.0f, -280.0f};
    m_space_hint->SetZIndex(Z_HINT);
    m_space_hint->UpdateDisplayText();

    m_no_enemy_text = std::make_shared<NumericDisplayText>(fontPath, 24);
    m_no_enemy_text->SetPrefix(AppUtil::GetGlobalString("msg_no_enemy", "No Enemies in Registry.")); m_no_enemy_text->SetShowNumber(false);
    m_no_enemy_text->m_Transform.translation = BG_POS; m_no_enemy_text->SetZIndex(Z_HINT);
    m_no_enemy_text->UpdateDisplayText();

    m_left_arrow = std::make_shared<Util::GameObject>(std::make_unique<Util::Image>(AppUtil::GetStaticResourcePath("bmp/Special/left_arrow_white.png")), Z_HINT);
    m_left_arrow->m_Transform.translation = {290.0f, -230.0f}; m_left_arrow->m_Transform.scale = {0.5f, 0.5f};

    m_right_arrow = std::make_shared<Util::GameObject>(std::make_unique<Util::Image>(AppUtil::GetStaticResourcePath("bmp/Special/right_arrow_white.png")), Z_HINT);
    m_right_arrow->m_Transform.translation = {360.0f, -230.0f}; m_right_arrow->m_Transform.scale = {0.5f, 0.5f};

    SetVisible(false);
}

void EnemyBookUI::Refresh() {
    UpdateEnemyList();
    m_current_page = 0;
    UpdatePage(m_current_page);
}

void EnemyBookUI::UpdateEnemyList() {
    m_unique_enemy_ids.clear();
    for (auto const& [id, meta] : AppUtil::GlobalObjectRegistry) {
        if (id >= 400 && id < 500) m_unique_enemy_ids.push_back(id);
    }
    std::sort(m_unique_enemy_ids.begin(), m_unique_enemy_ids.end());
    m_total_pages = (static_cast<int>(m_unique_enemy_ids.size()) + ENTRIES_PER_PAGE - 1) / ENTRIES_PER_PAGE;
}

void EnemyBookUI::UpdatePage(int pageIdx) {
    bool hasAny = (m_total_pages > 0);
    if (m_no_enemy_text) m_no_enemy_text->SetVisible(m_visible && !hasAny);
    
    for (int i = 0; i < ENTRIES_PER_PAGE; ++i) {
        int idx = pageIdx * ENTRIES_PER_PAGE + i;
        bool inRange = hasAny && (idx < static_cast<int>(m_unique_enemy_ids.size()));
        
        m_frames[i]->SetVisible(m_visible && inRange);
        m_icons[i]->SetVisible(m_visible && inRange);
        m_names[i]->SetVisible(m_visible && inRange);
        m_specials[i]->SetVisible(m_visible && inRange);
        m_hps[i]->SetVisible(m_visible && inRange);
        m_atks[i]->SetVisible(m_visible && inRange);
        m_defs[i]->SetVisible(m_visible && inRange);
        m_agis[i]->SetVisible(m_visible && inRange);
        m_atkTimes[i]->SetVisible(m_visible && inRange);
        m_damages[i]->SetVisible(m_visible && inRange);
        m_exps[i]->SetVisible(m_visible && inRange);
        m_coins[i]->SetVisible(m_visible && inRange);

        if (inRange) {
            int id = m_unique_enemy_ids[idx];
            auto const& meta = AppUtil::GlobalObjectRegistry[id];
            m_icons[i]->SetDrawable(std::make_shared<Util::Image>(AppUtil::GetFullResourcePath(id)));
            
            m_names[i]->SetPrefix(AppUtil::GetGlobalString("label_name", "Name: "));
            m_names[i]->SetSuffix(meta.GetString(AppUtil::Attr::TITLE));
            m_names[i]->SetShowNumber(false);

            m_specials[i]->SetPrefix(AppUtil::GetGlobalString("label_special", "Spec: "));
            m_specials[i]->SetSuffix(meta.GetString("Special", AppUtil::GetGlobalString("label_none", "None")));
            m_specials[i]->SetShowNumber(false);

            m_hps[i]->SetNumber(meta.GetInt(AppUtil::Attr::HP));
            m_atks[i]->SetNumber(meta.GetInt(AppUtil::Attr::ATTACK));
            m_defs[i]->SetNumber(meta.GetInt(AppUtil::Attr::DEFENSE));
            m_agis[i]->SetNumber(meta.GetInt(AppUtil::Attr::AGILITY));
            
            m_atkTimes[i]->SetPrefix(AppUtil::GetGlobalString("label_atk_time", "Times: "));
            m_atkTimes[i]->SetNumber(meta.GetInt("ATK_Time", 1));
            m_atkTimes[i]->SetShowNumber(true);
            
            long long dmg = AppUtil::CalculateDamage(m_player.get(), id);
            if (dmg < 0) { 
                m_damages[i]->SetPrefix(AppUtil::GetGlobalString("label_damage", "Dmg: ") + "???"); 
                m_damages[i]->SetShowNumber(false); 
            }
            else { 
                m_damages[i]->SetPrefix(AppUtil::GetGlobalString("label_damage", "Dmg: ")); 
                m_damages[i]->SetNumber(static_cast<int>(dmg)); 
                m_damages[i]->SetShowNumber(true); 
            }

            m_exps[i]->SetNumber(meta.GetInt(AppUtil::Attr::EXP));
            m_coins[i]->SetNumber(meta.GetInt(AppUtil::Attr::COIN));

            m_names[i]->UpdateDisplayText(); m_specials[i]->UpdateDisplayText();
            m_hps[i]->UpdateDisplayText(); m_atks[i]->UpdateDisplayText();
            m_defs[i]->UpdateDisplayText(); m_agis[i]->UpdateDisplayText();
            m_atkTimes[i]->UpdateDisplayText(); m_damages[i]->UpdateDisplayText();
            m_exps[i]->UpdateDisplayText(); m_coins[i]->UpdateDisplayText();
        }
    }

    if (m_left_arrow) m_left_arrow->SetVisible(m_visible && pageIdx > 0);
    if (m_right_arrow) m_right_arrow->SetVisible(m_visible && pageIdx < m_total_pages - 1);
}

void EnemyBookUI::run() {
    if (!m_visible) return;
    if (Util::Input::IsKeyDown(Util::Keycode::ESCAPE)) { SetVisible(false); return; }
    if (Util::Input::IsKeyDown(Util::Keycode::LEFT)) { if (m_current_page > 0) UpdatePage(--m_current_page); }
    else if (Util::Input::IsKeyDown(Util::Keycode::RIGHT)) { if (m_current_page < m_total_pages - 1) UpdatePage(++m_current_page); }
}

void EnemyBookUI::SetVisible(bool visible) {
    m_visible = visible;
    if (m_background) m_background->SetVisible(visible);
    if (m_space_hint) m_space_hint->SetVisible(visible);
    if (visible) Refresh();
    else {
        if (m_no_enemy_text) m_no_enemy_text->SetVisible(false);
        if (m_left_arrow) m_left_arrow->SetVisible(false);
        if (m_right_arrow) m_right_arrow->SetVisible(false);
        for (int i = 0; i < ENTRIES_PER_PAGE; ++i) {
            m_frames[i]->SetVisible(false); m_icons[i]->SetVisible(false);
            m_names[i]->SetVisible(false); m_specials[i]->SetVisible(false);
            m_hps[i]->SetVisible(false); m_atks[i]->SetVisible(false);
            m_defs[i]->SetVisible(false); m_agis[i]->SetVisible(false);
            m_atkTimes[i]->SetVisible(false); m_damages[i]->SetVisible(false);
            m_exps[i]->SetVisible(false); m_coins[i]->SetVisible(false);
        }
    }
}

void EnemyBookUI::AddToRoot(Util::Renderer& root) {
    if (m_background) root.AddChild(m_background);
    if (m_no_enemy_text) root.AddChild(m_no_enemy_text);
    for (int i = 0; i < ENTRIES_PER_PAGE; ++i) {
        root.AddChild(m_frames[i]); root.AddChild(m_icons[i]);
        root.AddChild(m_names[i]); root.AddChild(m_specials[i]);
        root.AddChild(m_hps[i]); root.AddChild(m_atks[i]);
        root.AddChild(m_defs[i]); root.AddChild(m_agis[i]);
        root.AddChild(m_atkTimes[i]); root.AddChild(m_damages[i]);
        root.AddChild(m_exps[i]); root.AddChild(m_coins[i]);
    }
    if (m_space_hint) root.AddChild(m_space_hint);
    if (m_left_arrow) root.AddChild(m_left_arrow);
    if (m_right_arrow) root.AddChild(m_right_arrow);
}
