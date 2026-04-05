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

    // Player Icon & Status text
    m_player_icon = std::make_shared<Util::GameObject>();
    m_player_icon->SetDrawable(std::make_shared<Util::Image>(AppUtil::GetStaticResourcePath("bmp/Player/player_1.png")));
    m_player_icon->m_Transform.translation = {-420.0f, 265.0f};
    m_player_icon->m_Transform.scale = {0.735f, 0.735f};
    m_player_icon->SetZIndex(-3.0f);

    m_status_text = MakeText(-300.0f, 250.0f, white, 24);
    m_status_text->SetShowNumber(false);
    m_status_text->SetPrefix(AppUtil::GetGlobalString("status_normal", "Normal"));
    m_status_text->UpdateDisplayText();
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

        // Update status and icon based on mode
        if (player->IsSuperMode()) {
            m_status_text->SetPrefix(AppUtil::GetGlobalString("status_super", "Super"));
            m_player_icon->SetDrawable(std::make_shared<Util::Image>(AppUtil::GetStaticResourcePath(AppUtil::Skin::SUPER_MODE_PATH)));
            m_player_icon->m_Transform.scale = {0.735f* AppUtil::Skin::SUPER_MODE_RATIO, 0.735f * AppUtil::Skin::SUPER_MODE_RATIO};
        } else {
            m_status_text->SetPrefix(AppUtil::GetGlobalString("status_normal", "Normal"));
            m_player_icon->SetDrawable(std::make_shared<Util::Image>(AppUtil::GetStaticResourcePath("bmp/Player/player_1.png")));
            m_player_icon->m_Transform.scale = {0.735f, 0.735f};
        }
        m_status_text->UpdateDisplayText();
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
    m_player_icon->SetVisible(visible);
    m_status_text->SetVisible(visible);
}

void StatusUI::AddToRoot(Util::Renderer& root) {
    for (auto& entry : m_stat_entries) {
        root.AddChild(entry.text);
    }
    root.AddChild(m_floor_text);
    root.AddChild(m_manual_hint_text);
    root.AddChild(m_player_icon);
    root.AddChild(m_status_text);
}
