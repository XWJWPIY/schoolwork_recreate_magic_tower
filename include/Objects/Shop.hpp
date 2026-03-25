#ifndef SHOP_HPP
#define SHOP_HPP

#include "Objects/Entity.hpp"
#include "Core/AppUtil.hpp"
#include "Systems/ShopSystem.hpp"
#include "UI/DialogueManager.hpp"
#include <functional>
#include <memory>
#include <string>

class Player;

/**
 * @brief Shop entity. Manages the full shop session lifecycle (Open, HandleInput, Close).
 * 
 * Logic previously in Tradeable is consolidated here. Special scaling (e.g. Greed God)
 * is handled within BuildShopData based on m_object_id.
 */
class Shop : public Entity {
public:
    using OpenCallback  = std::function<void(Shop&)>;
    using CloseCallback = std::function<void()>;

    Shop(int id, OpenCallback onOpen, CloseCallback onClose);
    ~Shop() override = default;

    // ── Interaction ────────────────────────────────────────────────────────
    void Reaction(std::shared_ptr<Player> player) override;

    // ── Session lifecycle ──────────────────────────────────────────────────
    void Open(std::shared_ptr<Player> player, DialogueManager& ui, int floor);
    void Close(DialogueManager& ui);
    bool IsOpen() const { return m_is_open; }

    int GetSelectionIndex() const { return m_selection; }

protected:
    /**
     * @brief Populate m_session_data from CSV. 
     * Handles specific scaling logic for IDs like Greed God (602).
     */
    void BuildShopData(int floor);

    bool CanAfford(const AppUtil::ShopOption& opt, const Player& player) const;
    void ExecutePurchase(const AppUtil::ShopOption& opt, std::shared_ptr<Player> player);

private:
    AppUtil::ShopData m_session_data;
    int m_transaction_count = 0;
    bool m_is_open = false;
    int  m_selection = 0;

    OpenCallback  m_on_open;
    CloseCallback m_on_close;
};

#endif // SHOP_HPP
