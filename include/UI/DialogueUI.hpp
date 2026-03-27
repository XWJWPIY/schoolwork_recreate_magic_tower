#ifndef DIALOGUE_UI_HPP
#define DIALOGUE_UI_HPP

#include "Util/GameObject.hpp"
#include "Util/Renderer.hpp"
#include "Util/Image.hpp"

#include <vector>
#include <string>
#include <memory>
#include <functional>

#include "UI/NumericDisplayText.hpp"
#include "Core/AppUtil.hpp"
#include "Systems/ScriptEngine.hpp"
#include "Systems/ShopSystem.hpp"
#include "UI/ShopUI.hpp"
#include "UI/UIComponent.hpp"

class ItemNoticeUI;
class Player;
class Entity;

class DialogueUI : public UIComponent {
public:
    enum class Mode {
        INACTIVE,
        SCRIPT,     // Linear dialogue
        SELECTION,  // Menu selection (Shop)
        NOTICE      // Item notice
    };

    using ScriptStep = ScriptEngine::ScriptStep;

    DialogueUI(std::shared_ptr<ItemNoticeUI> ui);
    virtual ~DialogueUI() = default;

    void StartScript(const std::string& name, std::shared_ptr<Entity> source = nullptr, bool isShop = false);
    void StartShop(const std::string& scriptName, const AppUtil::ShopData& shopData, std::function<void(int)> onSelect, std::shared_ptr<Entity> source = nullptr);
    void ReplaceScriptText(const std::string& target, const std::string& replacement);
    void RefreshShopOptions(const AppUtil::ShopData& shopData);
    void EndShopSelection();
    void ShowNotice(const std::string& text);
    
    void AddToRoot(Util::Renderer& root) override;
    void SetVisible(bool visible) override;
    
    // Core lifecycle (UIComponent Interface)
    void run() override;
    
    // Status
    bool IsActive() const override { return m_mode != Mode::INACTIVE; }
    bool IsIntercepting() const override { return IsActive(); }
    Mode GetMode() const { return m_mode; }

    void SetPlayer(std::shared_ptr<Player> player) { m_player = player; }

private:
    void HandleInput();
    void AdvanceScript();
    void ParseScript(const std::string& name) { m_engine.LoadScript(name); }
    void ExecuteCommand(const ScriptStep& step);
    void SetUIState(bool dialogueVisible);
    void ApplyDialogueLayout();
    void ApplyShopLayout();

private:
    Mode m_mode = Mode::INACTIVE;
    std::shared_ptr<ItemNoticeUI> m_ui;
    std::shared_ptr<Player> m_player;
    std::string m_script_name;
    bool m_is_shop_session = false;
    
    ScriptEngine m_engine;
    std::shared_ptr<Entity> m_source_entity;
    std::string m_last_speaker;
    std::string m_pending_notice;

    // Base UI Components
    std::shared_ptr<Util::GameObject> m_background;
    std::shared_ptr<Util::GameObject> m_npc_icon;
    std::shared_ptr<NumericDisplayText> m_name_text;
    std::shared_ptr<NumericDisplayText> m_content_text;
    std::shared_ptr<NumericDisplayText> m_space_prompt;

    // Shop/Selection Components
    std::unique_ptr<ShopUI> m_shop_ui;
    AppUtil::ShopData m_current_shop_data;
    std::function<void(int)> m_on_selection;
    float m_blink_timer = 0.0f;
};

#endif // DIALOGUE_UI_HPP
