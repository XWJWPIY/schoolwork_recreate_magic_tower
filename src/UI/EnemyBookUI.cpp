#include "UI/EnemyBookUI.hpp"
#include "Objects/Player.hpp"
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
    
    // Relative offsets within an entry 
    const glm::vec2 OFF_NAME    = {-165.0f, 40.0f};
    const glm::vec2 OFF_SPECIAL = {15.0f, 40.0f};

    const Util::Color CLR_GREEN  = Util::Color::FromRGB(0, 255, 0);
    const Util::Color CLR_RED    = Util::Color::FromRGB(255, 0, 0);
    const Util::Color CLR_PINK   = Util::Color::FromRGB(255, 105, 180);
    const Util::Color CLR_ORANGE = Util::Color::FromRGB(255, 165, 0);
    const Util::Color CLR_BLUE   = Util::Color::FromRGB(0, 191, 255);
    const Util::Color CLR_YELLOW = Util::Color::FromRGB(255, 255, 0);
    const Util::Color CLR_WHITE  = Util::Color::FromRGB(255, 255, 255);

    // Config table for the 8 numeric stats (index matches EnemyBookUI::STAT_COUNT)
    // { label_key, default_label, offset, color }
    struct StatCfg {
        const char* labelKey;
        const char* labelDefault;
        glm::vec2 offset;
        Util::Color color;
    };

    const StatCfg STAT_CONFIGS[] = {
        {"label_hp",       "HP: ",     {-165.0f,   0.0f}, CLR_GREEN},
        {"label_atk",      "ATK: ",    { -70.0f,   0.0f}, CLR_PINK},
        {"label_def",      "DEF: ",    {  55.0f,   0.0f}, CLR_ORANGE},
        {"label_agi",      "AGI: ",    { 160.0f,   0.0f}, CLR_GREEN},
        {"label_atk_time", "Times: ",  {-165.0f, -40.0f}, CLR_BLUE},
        {"label_damage",   "Damage: ", { -70.0f, -40.0f}, CLR_RED},
        {"label_exp",      "EXP: ",    {  55.0f, -40.0f}, CLR_WHITE},
        {"label_gold",     "GOLD: ",   { 160.0f, -40.0f}, CLR_YELLOW},
    };

    // Indices into stats[] for Update() logic
    enum StatIdx { HP=0, ATK, DEF, AGI, ATK_TIME, DAMAGE, EXP, GOLD };

    // Attribute keys to read from ObjectMetadata (matches StatIdx order)
    const std::string STAT_ATTR_KEYS[] = {
        AppUtil::Attr::HP, AppUtil::Attr::ATTACK, 
        AppUtil::Attr::DEFENSE, AppUtil::Attr::AGILITY,
        "ATK_Time", "", // damage is computed, not read directly
        AppUtil::Attr::EXP, AppUtil::Attr::COIN,
    };
}

// --- EnemyEntry Implementation ---

void EnemyBookUI::EnemyEntry::Initialize(const std::string& fontPath, float baseY, float bgX) {
    glm::vec2 basePos = {bgX, baseY};

    frame = std::make_shared<Util::GameObject>();
    frame->SetDrawable(std::make_shared<Util::Image>(AppUtil::GetStaticResourcePath("bmp/Special/EnemyFrame.bmp")));
    frame->m_Transform.translation = basePos + glm::vec2(ICON_OFFSET_X, 20.0f);
    frame->SetZIndex(Z_FRAME);

    icon = std::make_shared<Util::GameObject>();
    icon->m_Transform.translation = frame->m_Transform.translation;
    icon->SetZIndex(Z_ICON);

    auto initT = [&](const std::string& key, const std::string& def, const glm::vec2& off, const Util::Color& clr) {
        auto t = std::make_shared<NumericDisplayText>(fontPath, 22);
        t->SetAlignLeft(true);
        t->SetPrefix(AppUtil::GetGlobalString(key, def));
        t->SetColor(clr);
        t->m_Transform.translation = basePos + off;
        t->SetZIndex(Z_TEXT);
        t->UpdateDisplayText();
        return t;
    };

    name    = initT("label_name",    "Name: ",    OFF_NAME,    CLR_WHITE);
    special = initT("label_special", "Special: ", OFF_SPECIAL, CLR_RED);

    stats.resize(EnemyBookUI::STAT_COUNT);
    for (int i = 0; i < EnemyBookUI::STAT_COUNT; ++i) {
        stats[i] = initT(STAT_CONFIGS[i].labelKey, STAT_CONFIGS[i].labelDefault,
                         STAT_CONFIGS[i].offset, STAT_CONFIGS[i].color);
    }
}

void EnemyBookUI::EnemyEntry::SetVisible(bool v) {
    if (frame) frame->SetVisible(v);
    if (icon) icon->SetVisible(v);
    if (name) name->SetVisible(v);
    if (special) special->SetVisible(v);
    for (auto& s : stats) { if (s) s->SetVisible(v); }
}

void EnemyBookUI::EnemyEntry::AddToRoot(Util::Renderer& root) {
    root.AddChild(frame); 
    root.AddChild(icon);
    root.AddChild(name); 
    root.AddChild(special);
    for (auto& s : stats) root.AddChild(s);
}

void EnemyBookUI::EnemyEntry::Update(const AppUtil::ObjectMetadata& meta, Player* player) {
    icon->SetDrawable(std::make_shared<Util::Image>(AppUtil::GetFullResourcePath(meta.GetInt("Icon_ID", meta.GetInt(AppUtil::Attr::ID)))));
    
    name->SetPrefix(AppUtil::GetGlobalString("label_name", "Name: "));
    name->SetSuffix(meta.GetString(AppUtil::Attr::TITLE));
    name->SetShowNumber(false);

    special->SetPrefix(AppUtil::GetGlobalString("label_special", "Spec: "));
    special->SetSuffix(meta.GetString("Special", AppUtil::GetGlobalString("label_none", "None")));
    special->SetShowNumber(false);

    // Set numbers from metadata for direct-read stats
    stats[HP]->SetNumber(meta.GetInt(AppUtil::Attr::HP));
    stats[ATK]->SetNumber(meta.GetInt(AppUtil::Attr::ATTACK));
    stats[DEF]->SetNumber(meta.GetInt(AppUtil::Attr::DEFENSE));
    stats[AGI]->SetNumber(meta.GetInt(AppUtil::Attr::AGILITY));

    stats[ATK_TIME]->SetPrefix(AppUtil::GetGlobalString("label_atk_time", "Times: "));
    stats[ATK_TIME]->SetNumber(meta.GetInt("ATK_Time", 1));
    stats[ATK_TIME]->SetShowNumber(true);
    
    // Damage is computed
    long long dmg = AppUtil::CalculateDamage(player, meta.GetInt(AppUtil::Attr::ID));
    if (dmg < 0) { 
        stats[DAMAGE]->SetPrefix(AppUtil::GetGlobalString("label_damage", "Dmg: ") + "???"); 
        stats[DAMAGE]->SetShowNumber(false); 
    }
    else { 
        stats[DAMAGE]->SetPrefix(AppUtil::GetGlobalString("label_damage", "Dmg: ")); 
        stats[DAMAGE]->SetNumber(static_cast<int>(dmg)); 
        stats[DAMAGE]->SetShowNumber(true); 
    }

    stats[EXP]->SetNumber(meta.GetInt(AppUtil::Attr::EXP));
    stats[GOLD]->SetNumber(meta.GetInt(AppUtil::Attr::COIN));

    // Batch update all text
    name->UpdateDisplayText(); 
    special->UpdateDisplayText();
    for (auto& s : stats) s->UpdateDisplayText();
}

// --- EnemyBookUI Implementation ---

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
        EnemyEntry entry;
        entry.Initialize(fontPath, ENTRY_START_Y - i * ENTRY_SPACING_Y, BG_POS.x);
        m_entries.push_back(entry);
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
        if (id >= 400 && id < 500) {
            // If it's a giant monster part (has a Core_ID pointing elsewhere), skip it
            if (meta.GetInt("Core_ID", 0) != 0) continue;
            m_unique_enemy_ids.push_back(id);
        }
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
        
        m_entries[i].SetVisible(m_visible && inRange);

        if (inRange) {
            int id = m_unique_enemy_ids[idx];
            auto const& meta = AppUtil::GlobalObjectRegistry[id];
            m_entries[i].Update(meta, m_player.get());
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
            m_entries[i].SetVisible(false);
        }
    }
}

void EnemyBookUI::AddToRoot(Util::Renderer& root) {
    if (m_background) root.AddChild(m_background);
    if (m_no_enemy_text) root.AddChild(m_no_enemy_text);
    for (int i = 0; i < ENTRIES_PER_PAGE; ++i) {
        m_entries[i].AddToRoot(root);
    }
    if (m_space_hint) root.AddChild(m_space_hint);
    if (m_left_arrow) root.AddChild(m_left_arrow);
    if (m_right_arrow) root.AddChild(m_right_arrow);
}
