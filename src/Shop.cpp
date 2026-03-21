#include "Shop.hpp"
#include "Player.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

Shop::Shop(int id, OpenCallback onOpen, CloseCallback onClose)
    : Entity(id, "", true),
      m_on_open(std::move(onOpen)),
      m_on_close(std::move(onClose)) {}

void Shop::Reaction(std::shared_ptr<Player> player) {
    LOG_INFO("Shop::Reaction id={} ({})", m_object_id, AppUtil::GetIdString(m_object_id));
    player->SetPendingShop(m_object_id);
}

// ── Session lifecycle ──────────────────────────────────────────────────────

void Shop::Open(std::shared_ptr<Player> player, MenuUI& ui) {
    auto it = AppUtil::GlobalObjectRegistry.find(m_object_id);
    if (it != AppUtil::GlobalObjectRegistry.end() && it->second.shop_props) {
        m_transaction_count = it->second.shop_props->transaction_count;
        if (it->second.shop_props->title == "None") {
            LOG_INFO("Shop::Open: shop {} title=None, skipping UI.", m_object_id);
            return;
        }
    }

    BuildShopData();

    m_selection = 0;
    m_is_open = true;

    ui.SetVisible(true, MenuUI::MenuType::SHOP);
    ui.SetShopData(m_session_data);
    ui.UpdateShopSelection(m_selection);

    LOG_INFO("Shop::Open id={}", m_object_id);
    if (m_on_open) m_on_open(*this);
}

void Shop::Close(MenuUI& ui) {
    m_is_open = false;
    ui.SetVisible(false);
    LOG_INFO("Shop::Close id={}", m_object_id);
    if (m_on_close) m_on_close();
}

// ── Per-frame input ────────────────────────────────────────────────────────

void Shop::HandleInput(std::shared_ptr<Player> player, MenuUI& ui) {
    if (!m_is_open || !player) return;

    const int opt_count = static_cast<int>(m_session_data.options.size());

    if (Util::Input::IsKeyDown(Util::Keycode::W) || Util::Input::IsKeyDown(Util::Keycode::UP)) {
        m_selection = (m_selection - 1 + opt_count) % opt_count;
        ui.UpdateShopSelection(m_selection);
    }
    if (Util::Input::IsKeyDown(Util::Keycode::S) || Util::Input::IsKeyDown(Util::Keycode::DOWN)) {
        m_selection = (m_selection + 1) % opt_count;
        ui.UpdateShopSelection(m_selection);
    }
    if (Util::Input::IsKeyDown(Util::Keycode::SPACE) || Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
        if (m_selection >= 0 && m_selection < opt_count) {
            const auto& opt = m_session_data.options[m_selection];
            if (opt.text == "Exit") {
                Close(ui);
                return;
            }
            if (CanAfford(opt, *player)) {
                ExecutePurchase(opt, player);
                auto it = AppUtil::GlobalObjectRegistry.find(m_object_id);
                if (it != AppUtil::GlobalObjectRegistry.end() && it->second.shop_props) {
                    it->second.shop_props->transaction_count = m_transaction_count;
                }
                BuildShopData();
                ui.SetShopData(m_session_data);
                ui.UpdateShopSelection(m_selection);
            }
        }
    }
    if (Util::Input::IsKeyDown(Util::Keycode::ESCAPE) || Util::Input::IsKeyDown(Util::Keycode::Q)) {
        Close(ui);
    }
}

// ── Protected helpers ──────────────────────────────────────────────────────

bool Shop::CanAfford(const AppUtil::ShopOption& opt, const Player& player) const {
    for (const auto& eff : opt.effects) {
        if (eff.value >= 0) continue;
        int cost = -eff.value;
        switch (eff.type) {
            case AppUtil::Effect::COIN: if (player.GetCoins() < cost) return false; break;
            case AppUtil::Effect::EXP: if (player.GetExp() < cost) return false; break;
            case AppUtil::Effect::HP: if (player.GetHp() <= cost) return false; break;
            case AppUtil::Effect::KEY_YELLOW: if (player.GetYellowKeys() < cost) return false; break;
            case AppUtil::Effect::KEY_BLUE: if (player.GetBlueKeys() < cost) return false; break;
            case AppUtil::Effect::KEY_RED: if (player.GetRedKeys() < cost) return false; break;
            default: break;
        }
    }
    return true;
}

void Shop::ExecutePurchase(const AppUtil::ShopOption& opt, std::shared_ptr<Player> player) {
    for (const auto& eff : opt.effects) player->ApplyEffect(eff.type, eff.value);
    m_transaction_count++;
    LOG_INFO("Shop purchase id={} trans={}", m_object_id, m_transaction_count);
}

void Shop::BuildShopData() {
    auto it = AppUtil::GlobalObjectRegistry.find(m_object_id);
    if (it == AppUtil::GlobalObjectRegistry.end() || !it->second.shop_props) return;

    const auto& shop = *it->second.shop_props;
    m_session_data.title = shop.title;
    m_session_data.icon_path = shop.icon_path;
    m_session_data.transaction_count = m_transaction_count;
    m_session_data.prompts.clear();

    std::string name = it->second.name;
    std::string dialogue_path = std::string(MAGIC_TOWER_RESOURCE_DIR) + "/Datas/Texts/" + name + ".csv";
    auto rows = AppUtil::MapParser::ParseCsvToStrings(dialogue_path);
    for (const auto& row : rows) if (!row.empty() && !row[0].empty()) m_session_data.prompts.push_back(row[0]);

    std::string option_path = std::string(MAGIC_TOWER_RESOURCE_DIR) + "/Datas/Texts/" + name + "_option.csv";
    m_session_data.options = AppUtil::MapParser::ParseShopOptions(option_path);
    if (m_session_data.options.empty()) m_session_data.options.push_back({"No Inventory Found", {}});
    m_session_data.options.push_back({"Exit", {}});

    // Dynamic Price Scaling for Greed God (ID 602)
    if (m_object_id == 602) {
        int cost = 20 + m_transaction_count + (m_transaction_count > 25 ? (m_transaction_count - 25) * 4 : 0);
        m_session_data.prompts.push_back(std::to_string(cost));
        for (auto& prompt : m_session_data.prompts) {
            size_t pos = prompt.find("　　　");
            if (pos != std::string::npos) {
                std::string price_str = std::to_string(cost);
                if (price_str.length() == 2) price_str = " " + price_str;
                prompt.replace(pos, 9, price_str);
            }
        }
        for (auto& optr : m_session_data.options) {
            if (optr.text == "Exit") continue;
            for (auto& eff : optr.effects) if (eff.type == AppUtil::Effect::COIN) eff.value = -cost;
            size_t pos = optr.text.find('(');
            if (pos != std::string::npos) optr.text = optr.text.substr(0, pos);
        }
    }
}
