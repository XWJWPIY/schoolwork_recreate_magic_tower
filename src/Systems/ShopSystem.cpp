#include "Systems/ShopSystem.hpp"
#include "Core/AppUtil.hpp"
#include "Util/Logger.hpp"

AppUtil::ShopData ShopSystem::LoadFromStaticFile(const std::string& name) {
    AppUtil::ShopData data;
    data.options = ParseShopOptions(GetOptionPath(name));
    AddDefaultOptions(data);
    return data;
}

AppUtil::ShopData ShopSystem::LoadForShopEntity(int shopId, int floor, int transCount) {
    AppUtil::ShopData data;
    auto it = AppUtil::GlobalObjectRegistry.find(shopId);
    if (it == AppUtil::GlobalObjectRegistry.end()) {
        LOG_ERROR("ShopSystem: Shop ID {} not found in registry", shopId);
        return data;
    }

    const auto& meta = it->second;
    data.title = meta.GetString(AppUtil::Attr::TITLE, "Store Explorer");
    data.icon_path = meta.GetString(AppUtil::Attr::ICON);
    data.transaction_count = transCount;

    std::string name = std::to_string(floor) + "_" + meta.name + "_option";
    data.options = ParseShopOptions(GetOptionPath(name));
    AddDefaultOptions(data);

    ApplyDynamicPricing(shopId, transCount, data);
    return data;
}

void ShopSystem::ApplyDynamicPricing(int shopId, int transCount, AppUtil::ShopData& data) {
    if (shopId != 602) {
        data.special_price_str = "";
        return;
    }

    // Scaling Greed God pricing (ID 602)
    // Formula: 20 + count + (if > 25, additional 4 per trans)
    int baseCost = 20 + transCount;
    int extraBonus = (transCount > 25) ? (transCount - 25) * 4 : 0;
    int totalCost = baseCost + extraBonus;
    
    // Format price string for UI alignment (4 characters wide)
    char buf[8];
    snprintf(buf, sizeof(buf), "%4d", totalCost);
    data.special_price_str = buf;

    for (auto& opt : data.options) {
        if (opt.text == "Exit") continue;

        // Apply price to all effects with COIN type
        for (auto& eff : opt.effects) {
            if (AppUtil::AttributeRegistry::ToEffect(eff.type_id) == AppUtil::Effect::COIN) {
                eff.value = -totalCost;
            }
        }

        // Clean up UI text (remove old price if it was included in the CSV text)
        size_t bracketPos = opt.text.find('(');
        if (bracketPos != std::string::npos) {
            opt.text = opt.text.substr(0, bracketPos);
        }
    }
}

std::vector<AppUtil::ShopOption> ShopSystem::ParseShopOptions(const std::string& filepath) {
    std::vector<AppUtil::ShopOption> options;
    AppUtil::CSVLoader loader;
    if (!loader.Load(filepath)) return options;

    for (size_t i = 0; i < loader.GetRowCount(); ++i) {
        AppUtil::ShopOption opt;
        opt.text = loader.GetString(i, AppUtil::Attr::DIALOG);
        opt.effects = loader.GetRowEffects(i);

        if (!opt.effects.empty() || !opt.text.empty()) {
            options.push_back(std::move(opt));
        }
    }
    return options;
}

std::string ShopSystem::GetOptionPath(const std::string& name) {
    return AppUtil::GetStaticResourcePath("Datas/Texts/" + name + ".csv");
}

void ShopSystem::AddDefaultOptions(AppUtil::ShopData& data) {
    if (data.options.empty()) {
        data.options.push_back({"No Inventory Found", {}});
    }
    // Always append Exit at the end
    data.options.push_back({"Exit", {}});
}
