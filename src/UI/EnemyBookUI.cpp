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

    auto initT = [&](std::shared_ptr<NumericDisplayText>& t, const std::string& key, const std::string& def, const glm::vec2& off, const Util::Color& clr) {
        t = std::make_shared<NumericDisplayText>(fontPath, 22);
        t->SetAlignLeft(true);
        t->SetPrefix(AppUtil::GetGlobalString(key, def));
        t->SetColor(clr);
        t->m_Transform.translation = basePos + off;
        t->SetZIndex(Z_TEXT);
        t->UpdateDisplayText();
    };

    initT(name,     "label_name", "Name: ", OFF_NAME,    CLR_WHITE);
    initT(special,  "label_special", "Special: ", OFF_SPECIAL, CLR_RED);
    initT(hp,       "label_hp", "HP: ",   OFF_HP,      CLR_GREEN);
    initT(atk,      "label_atk", "ATK: ",  OFF_ATK,     CLR_PINK);
    initT(def,      "label_def", "DEF: ",  OFF_DEF,     CLR_ORANGE);
    initT(agi,      "label_agi", "AGI: ",  OFF_AGI,     CLR_GREEN);
    initT(atkTime,  "label_atk_time", "Times: ", OFF_ATK_TIME,CLR_BLUE);
    initT(damage,   "label_damage", "Damage: ", OFF_DAMAGE,  CLR_RED);
    initT(exp,      "label_exp", "EXP: ",  OFF_EXP,     CLR_WHITE);
    initT(gold,     "label_gold", "GOLD: ", OFF_COIN,    CLR_YELLOW);
}

void EnemyBookUI::EnemyEntry::SetVisible(bool v) {
    if (frame) frame->SetVisible(v);
    if (icon) icon->SetVisible(v);
    if (name) name->SetVisible(v);
    if (special) special->SetVisible(v);
    if (hp) hp->SetVisible(v);
    if (atk) atk->SetVisible(v);
    if (def) def->SetVisible(v);
    if (agi) agi->SetVisible(v);
    if (atkTime) atkTime->SetVisible(v);
    if (damage) damage->SetVisible(v);
    if (exp) exp->SetVisible(v);
    if (gold) gold->SetVisible(v);
}

void EnemyBookUI::EnemyEntry::AddToRoot(Util::Renderer& root) {
    root.AddChild(frame); root.AddChild(icon);
    root.AddChild(name); root.AddChild(special);
    root.AddChild(hp); root.AddChild(atk);
    root.AddChild(def); root.AddChild(agi);
    root.AddChild(atkTime); root.AddChild(damage);
    root.AddChild(exp); root.AddChild(gold);
}

void EnemyBookUI::EnemyEntry::Update(const AppUtil::ObjectMetadata& meta, Player* player) {
    icon->SetDrawable(std::make_shared<Util::Image>(AppUtil::GetFullResourcePath(meta.GetInt(AppUtil::Attr::ID))));
    
    name->SetPrefix(AppUtil::GetGlobalString("label_name", "Name: "));
    name->SetSuffix(meta.GetString(AppUtil::Attr::TITLE));
    name->SetShowNumber(false);

    special->SetPrefix(AppUtil::GetGlobalString("label_special", "Spec: "));
    special->SetSuffix(meta.GetString("Special", AppUtil::GetGlobalString("label_none", "None")));
    special->SetShowNumber(false);

    hp->SetNumber(meta.GetInt(AppUtil::Attr::HP));
    atk->SetNumber(meta.GetInt(AppUtil::Attr::ATTACK));
    def->SetNumber(meta.GetInt(AppUtil::Attr::DEFENSE));
    agi->SetNumber(meta.GetInt(AppUtil::Attr::AGILITY));
    
    atkTime->SetPrefix(AppUtil::GetGlobalString("label_atk_time", "Times: "));
    atkTime->SetNumber(meta.GetInt("ATK_Time", 1));
    atkTime->SetShowNumber(true);
    
    long long dmg = AppUtil::CalculateDamage(player, meta.GetInt(AppUtil::Attr::ID));
    if (dmg < 0) { 
        damage->SetPrefix(AppUtil::GetGlobalString("label_damage", "Dmg: ") + "???"); 
        damage->SetShowNumber(false); 
    }
    else { 
        damage->SetPrefix(AppUtil::GetGlobalString("label_damage", "Dmg: ")); 
        damage->SetNumber(static_cast<int>(dmg)); 
        damage->SetShowNumber(true); 
    }

    exp->SetNumber(meta.GetInt(AppUtil::Attr::EXP));
    gold->SetNumber(meta.GetInt(AppUtil::Attr::COIN));

    name->UpdateDisplayText(); special->UpdateDisplayText();
    hp->UpdateDisplayText(); atk->UpdateDisplayText();
    def->UpdateDisplayText(); agi->UpdateDisplayText();
    atkTime->UpdateDisplayText(); damage->UpdateDisplayText();
    exp->UpdateDisplayText(); gold->UpdateDisplayText();
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
