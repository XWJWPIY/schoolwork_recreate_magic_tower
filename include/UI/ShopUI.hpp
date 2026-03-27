#ifndef SHOPUI_HPP
#define SHOPUI_HPP

#include "Util/GameObject.hpp"
#include "Util/Renderer.hpp"
#include "UI/NumericDisplayText.hpp"
#include "Systems/ShopSystem.hpp"
#include "Core/AppUtil.hpp"
#include "UI/UIComponent.hpp"
#include <vector>
#include <memory>
#include <functional>

class ShopUI : public UIComponent {
public:
    ShopUI(const std::string& fontPath);
    
    /**
     * @brief Start a shop session with given data and callback.
     */
    void Start(const AppUtil::ShopData& data, std::function<void(int)> onSelect);
    
    /**
     * @brief Update the internal selection and visuals.
     */
    /**
     * @brief Sync UI components to the root renderer.
     */
    void AddToRoot(Util::Renderer& root) override;
    
    /**
     * @brief Set visibility of all shop-specific UI elements.
     */
    void SetVisible(bool visible) override;

    /**
     * @brief Core lifecycle
     */
    void run() override;
    bool IsActive() const override { return m_visible; }
    bool IsIntercepting() const override { return IsActive(); }
    
    /**
     * @brief Refresh displayed options (e.g., after a purchase).
     */
    void Refresh(const AppUtil::ShopData& data);

    int GetSelection() const { return m_selection; }

private:
    void UpdateSelectionVisuals();

    std::vector<std::shared_ptr<NumericDisplayText>> m_options;
    std::shared_ptr<Util::GameObject> m_selector;
    std::shared_ptr<NumericDisplayText> m_price_display;
    
    AppUtil::ShopData m_data;
    int m_selection = 0;
    std::function<void(int)> m_on_select;
    bool m_visible = false;
};

#endif // SHOPUI_HPP
