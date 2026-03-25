#ifndef DIALOGUEMANAGER_HPP
#define DIALOGUEMANAGER_HPP

#include "Util/GameObject.hpp"
#include "Util/Renderer.hpp"

#include "AppUtil.hpp"
#include <vector>
#include <string>
#include <memory>
#include <functional>

#include "NumericDisplayText.hpp"
#include "ScriptEngine.hpp"
#include "ShopSystem.hpp"
#include "ShopUI.hpp"
#include "Util/Image.hpp"

class MenuUI;
class Player;
class Entity;

class DialogueManager {
public:
    enum class Mode {
        INACTIVE,
        SCRIPT,     // Linear dialogue (Elder, etc.)
        SELECTION,  // Menu selection (Shop, etc.)
        NOTICE      // One-line item notice
    };

    using ScriptStep = ScriptEngine::ScriptStep;

    DialogueManager(std::shared_ptr<MenuUI> ui);
    virtual ~DialogueManager() = default;

    void StartScript(const std::string& name, std::shared_ptr<Entity> source = nullptr, bool isShop = false);
    void StartShop(const std::string& scriptName, const AppUtil::ShopData& shopData, std::function<void(int)> onSelect, std::shared_ptr<Entity> source = nullptr);
    void ReplaceScriptText(const std::string& target, const std::string& replacement);
    void RefreshShopOptions(const AppUtil::ShopData& shopData);
    void EndShopSelection();
    void ShowNotice(const std::string& text);
    
    void AddToRoot(Util::Renderer& root);
    void SetVisible(bool visible);
    
    // Core lifecycle
    void HandleInput(std::shared_ptr<Player> player);
    void Update();
    
    // Status
    bool IsActive() const { return m_mode != Mode::INACTIVE; }
    Mode GetMode() const { return m_mode; }

private:
    void AdvanceScript(std::shared_ptr<Player> player);
    void ParseScript(const std::string& name) { m_engine.LoadScript(name); }
    void ExecuteCommand(const ScriptStep& step, std::shared_ptr<Player> player);
    void SetUIState(bool dialogueVisible);
    void ApplyDialogueLayout();
    void ApplyShopLayout();

private:
    Mode m_mode = Mode::INACTIVE;
    std::shared_ptr<MenuUI> m_ui;
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
};

#endif // DIALOGUEMANAGER_HPP
