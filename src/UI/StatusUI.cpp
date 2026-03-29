#include "UI/StatusUI.hpp"
#include "Objects/Player.hpp"
#include "Core/FloorMap.hpp"
#include "UI/NumericDisplayText.hpp"
#include "Core/AppUtil.hpp"

// Config table: { Effect, x, y, fontSize }
struct StatConfig {
    AppUtil::Effect effect;
    float x, y;
    unsigned int fontSize;
};

StatusUI::StatusUI(const std::shared_ptr<Player>& player, 
                   const std::shared_ptr<FloorMap>& floorMap,
                   unsigned int fontSize) 
    : m_default_font_size(fontSize), m_player(player), m_road_map(floorMap) {
    m_visible = true; // StatusUI defaults to visible

    Util::Color white = Util::Color::FromRGB(255, 255, 255);

    // Table-driven stat initialization
    const StatConfig configs[] = {
        // Keys & Coin (larger font)
        {AppUtil::Effect::KEY_YELLOW, -290.0f, -115.0f, m_default_font_size},
        {AppUtil::Effect::KEY_BLUE,   -290.0f, -172.0f, m_default_font_size},
        {AppUtil::Effect::KEY_RED,    -290.0f, -228.0f, m_default_font_size},
        {AppUtil::Effect::COIN,       -290.0f, -284.0f, m_default_font_size},
        // Player stats (smaller font)
        {AppUtil::Effect::LEVEL,    -277.0f, 209.0f, 24},
        {AppUtil::Effect::HP,       -277.0f, 166.0f, 24},
        {AppUtil::Effect::ATTACK,   -277.0f, 124.0f, 24},
        {AppUtil::Effect::DEFENSE,  -277.0f,  83.0f, 24},
        {AppUtil::Effect::AGILITY,  -277.0f,  41.0f, 24},
        {AppUtil::Effect::EXP,      -277.0f,   0.0f, 24},
    };

    for (const auto& cfg : configs) {
        StatEntry entry;
        entry.effect = cfg.effect;
        entry.text = MakeText(cfg.x, cfg.y, white, cfg.fontSize);
        m_stat_entries.push_back(entry);
    }

    // Floor display
    m_floor_text = MakeText(150.0f, 335.0f, white, 32);
    m_floor_text->SetSuffix(" F");

    // Manual hint
    m_manual_hint_text = MakeText(390.0f, -335.0f, Util::Color::FromRGB(0, 0, 0), 24);
    m_manual_hint_text->SetPrefix("-Press (L)-");
    m_manual_hint_text->SetShowNumber(false);
    m_manual_hint_text->UpdateDisplayText();
}

std::shared_ptr<NumericDisplayText> StatusUI::MakeText(float x, float y, const Util::Color& color, unsigned int size) {
    auto text = std::make_shared<NumericDisplayText>(
        AppUtil::GetStaticResourcePath("Font/Cubic_11.ttf"), size);
    text->SetPrefix("");
    text->SetNumber(0);
    text->SetColor(color);
    text->m_Transform.translation = {x, y};
    text->SetZIndex(-3.0f);
    text->SetVisible(false);
    return text;
}

void StatusUI::run() {
    if (!m_visible) return;
    int floor = (m_road_map ? m_road_map->GetCurrentStory() : 0);
    Update(m_player, floor);
}

void StatusUI::Update(const std::shared_ptr<Player>& player, int floorNum) {
    if (player) {
        for (auto& entry : m_stat_entries) {
            entry.text->SetNumber(player->GetAttr(entry.effect));
            entry.text->UpdateDisplayText();
        }
    }

    m_floor_text->SetNumber(floorNum);
    m_floor_text->UpdateDisplayText();
}

void StatusUI::SetVisible(bool visible) {
    m_visible = visible;
    for (auto& entry : m_stat_entries) {
        entry.text->SetVisible(visible);
    }
    m_floor_text->SetVisible(visible);
    m_manual_hint_text->SetVisible(visible);
}

void StatusUI::AddToRoot(Util::Renderer& root) {
    for (auto& entry : m_stat_entries) {
        root.AddChild(entry.text);
    }
    root.AddChild(m_floor_text);
    root.AddChild(m_manual_hint_text);
}
