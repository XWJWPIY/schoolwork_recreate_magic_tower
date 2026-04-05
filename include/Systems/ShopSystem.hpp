#ifndef SHOPSYSTEM_HPP
#define SHOPSYSTEM_HPP

#include "Core/AppUtil.hpp"
#include <string>
#include <vector>

namespace AppUtil {
    struct ShopOption {
        std::string text;
        std::vector<SubEffect> effects;
    };

    struct ShopData {
        std::string title;
        std::string icon_path;
        std::vector<std::string> prompts;
        int transaction_count;
        int max_transactions = -1; // -1 means unlimited
        std::vector<ShopOption> options;
        std::string special_price_str;
    };
}

class ShopSystem {
public:
    /**
     * @brief Load shop data from a CSV file.
     * @param name The filename without extension (e.g., "5_shopkeeper_option")
     */
    static AppUtil::ShopData LoadFromStaticFile(const std::string& name);

    /**
     * @brief Load shop data for a specific shop entity at a floor.
     * Handles metadata lookup and dynamic pricing.
     */
    static AppUtil::ShopData LoadForShopEntity(int shopId, int floor, int transCount);

private:
    /**
     * @brief Handle dynamic price scaling for specific shops (e.g., Greed God)
     */
    static void ApplyDynamicPricing(int shopId, int transCount, AppUtil::ShopData& data);

    /**
     * @brief Core CSV parsing for shop options.
     */
    static std::vector<AppUtil::ShopOption> ParseShopOptions(const std::string& filepath);

    /**
     * @brief Build the full path to a shop option CSV.
     */
    static std::string GetOptionPath(const std::string& name);

    /**
     * @brief Add "Exit" and "Empty" fallbacks to shop options.
     */
    static void AddDefaultOptions(AppUtil::ShopData& data);
};

#endif // SHOPSYSTEM_HPP
