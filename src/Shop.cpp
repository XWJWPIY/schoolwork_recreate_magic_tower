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

void Shop::Open(std::shared_ptr<Player> player, DialogueManager& diag, int floor) {
    auto it = AppUtil::GlobalObjectRegistry.find(m_object_id);
    if (it != AppUtil::GlobalObjectRegistry.end() && it->second.shop_props) {
        m_transaction_count = it->second.shop_props->transaction_count;
        if (it->second.shop_props->title == "None") {
            LOG_INFO("Shop::Open: shop {} title=None, skipping UI.", m_object_id);
            return;
        }
    }

    BuildShopData(floor);
    m_selection = 0;
    m_is_open = true;

    auto onSelect = [this, &diag, player, floor](int selection) {
        if (selection < 0 || selection >= static_cast<int>(m_session_data.options.size())) return;
        const auto& opt = m_session_data.options[selection];
        
        if (opt.text == "Exit") {
            Close(diag);
            return;
        }
        
        if (CanAfford(opt, *player)) {
            ExecutePurchase(opt, player);
            auto registry_it = AppUtil::GlobalObjectRegistry.find(m_object_id);
            if (registry_it != AppUtil::GlobalObjectRegistry.end() && registry_it->second.shop_props) {
                registry_it->second.shop_props->transaction_count = m_transaction_count;
            }
            BuildShopData(floor);
            diag.RefreshShopOptions(m_session_data);
        }
    };

    std::string name = std::to_string(floor) + "_" + AppUtil::GetIdString(m_object_id);
    diag.StartShop(name, m_session_data, onSelect, nullptr);

    LOG_INFO("Shop::Open id={}", m_object_id);
    if (m_on_open) m_on_open(*this);
}

void Shop::Close(DialogueManager& diag) {
    m_is_open = false;
    diag.EndShopSelection();
    LOG_INFO("Shop::Close id={}", m_object_id);
    if (m_on_close) m_on_close();
}

bool Shop::CanAfford(const AppUtil::ShopOption& opt, const Player& player) const {
    for (const auto& eff : opt.effects) {
        if (eff.value >= 0) continue;
        int cost = -eff.value;
        AppUtil::Effect type = AppUtil::AttributeRegistry::ToEffect(eff.type_id);
        switch (type) {
            case AppUtil::Effect::COIN: if (player.GetAttr(AppUtil::Effect::COIN) < cost) return false; break;
            case AppUtil::Effect::EXP: if (player.GetAttr(AppUtil::Effect::EXP) < cost) return false; break;
            case AppUtil::Effect::HP: if (player.GetAttr(AppUtil::Effect::HP) <= cost) return false; break;
            case AppUtil::Effect::KEY_YELLOW: if (player.GetAttr(AppUtil::Effect::KEY_YELLOW) < cost) return false; break;
            case AppUtil::Effect::KEY_BLUE: if (player.GetAttr(AppUtil::Effect::KEY_BLUE) < cost) return false; break;
            case AppUtil::Effect::KEY_RED: if (player.GetAttr(AppUtil::Effect::KEY_RED) < cost) return false; break;
            default: break;
        }
    }
    return true;
}

void Shop::ExecutePurchase(const AppUtil::ShopOption& opt, std::shared_ptr<Player> player) {
    for (const auto& eff : opt.effects) {
        player->ApplyEffect(AppUtil::AttributeRegistry::ToEffect(eff.type_id), eff.value);
    }
    m_transaction_count++;
    LOG_INFO("Shop purchase id={} trans={}", m_object_id, m_transaction_count);
}

void Shop::BuildShopData(int floor) {
    auto it = AppUtil::GlobalObjectRegistry.find(m_object_id);
    if (it == AppUtil::GlobalObjectRegistry.end() || !it->second.shop_props) return;

    const auto& shop = *it->second.shop_props;
    m_session_data.title = shop.title;
    m_session_data.icon_path = shop.icon_path;
    m_session_data.transaction_count = m_transaction_count;
    m_session_data.prompts.clear();

    std::string name = std::to_string(floor) + "_" + it->second.name;
    std::string option_path = std::string(MAGIC_TOWER_RESOURCE_DIR) + "/Datas/Texts/" + name + "_option.csv";
    m_session_data.options = AppUtil::MapParser::ParseShopOptions(option_path);
    if (m_session_data.options.empty()) m_session_data.options.push_back({"No Inventory Found", {}});
    m_session_data.options.push_back({"Exit", {}});

    // Calculate Dynamic Price
    if (shop.pricing_type == AppUtil::ShopPricingType::SCALING_GREED) {
        int cost = 20 + m_transaction_count + (m_transaction_count > 25 ? (m_transaction_count - 25) * 4 : 0);
        
        // Update Special Price String for Dialogue UI
        std::string price_str = std::to_string(cost);
        if (price_str.length() == 1) price_str = "   " + price_str;
        else if (price_str.length() == 2) price_str = "  " + price_str;
        else if (price_str.length() == 3) price_str = " " + price_str;
        m_session_data.special_price_str = price_str;

        // Update Option Costs
        for (auto& optr : m_session_data.options) {
            if (optr.text == "Exit") continue;
            for (auto& eff : optr.effects) {
                if (AppUtil::AttributeRegistry::ToEffect(eff.type_id) == AppUtil::Effect::COIN) eff.value = -cost;
            }
            size_t pos = optr.text.find('(');
            if (pos != std::string::npos) optr.text = optr.text.substr(0, pos);
        }
    } else {
        m_session_data.special_price_str = "";
    }
}
