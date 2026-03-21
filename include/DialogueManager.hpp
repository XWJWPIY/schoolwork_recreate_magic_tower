#ifndef DIALOGUEMANAGER_HPP
#define DIALOGUEMANAGER_HPP

#include "AllObjects.hpp"
#include <vector>
#include <string>
#include <memory>
#include <functional>

class MenuUI;
class Player;
class Entity;

class DialogueManager : public AllObjects {
public:
    enum class Mode {
        INACTIVE,
        SCRIPT,     // Linear dialogue (Elder, etc.)
        SELECTION,  // Menu selection (Shop, etc.)
        NOTICE      // One-line item notice
    };

    struct ScriptLine {
        std::string command; // e.g., "speaker name", "item", "hide", "menu"
        std::string content;
        std::string extra;   // Used for item IDs or menu paths
    };

    DialogueManager(std::shared_ptr<MenuUI> ui);
    virtual ~DialogueManager() = default;

    void StartScript(const std::string& name, std::shared_ptr<Entity> source = nullptr);
    void ShowNotice(const std::string& text);
    
    // Core lifecycle
    void HandleInput(std::shared_ptr<Player> player);
    void ObjectUpdate() override;
    
    // Status
    bool IsActive() const { return m_mode != Mode::INACTIVE; }
    Mode GetMode() const { return m_mode; }

private:
    void AdvanceScript(std::shared_ptr<Player> player);
    void ParseScript(const std::string& name);
    void ExecuteCommand(const ScriptLine& line, std::shared_ptr<Player> player);

private:
    Mode m_mode = Mode::INACTIVE;
    std::shared_ptr<MenuUI> m_ui;
    
    std::vector<ScriptLine> m_script;
    int m_current_line = 0;
    
    std::string m_last_speaker;
    std::shared_ptr<Entity> m_source_entity;
};

#endif // DIALOGUEMANAGER_HPP
