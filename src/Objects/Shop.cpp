#include "Objects/Shop.hpp"
#include "Objects/Player.hpp"
#include "UI/DialogueManager.hpp"
#include "Systems/ShopSystem.hpp"
#include "Core/AppUtil.hpp"
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
    if (it != AppUtil::GlobalObjectRegistry.end()) {
        m_transaction_count = it->second.GetInt(AppUtil::Attr::TRANSACTIONS);
        if (it->second.GetString(AppUtil::Attr::TITLE) == "None") {
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
            if (registry_it != AppUtil::GlobalObjectRegistry.end()) {
                registry_it->second.attributes[AppUtil::AttributeRegistry::GetId(AppUtil::Attr::TRANSACTIONS)] = std::to_string(m_transaction_count);
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
    m_session_data = ShopSystem::LoadForShopEntity(m_object_id, floor, m_transaction_count);
}
