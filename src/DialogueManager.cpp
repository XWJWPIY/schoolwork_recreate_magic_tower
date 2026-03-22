#include "DialogueManager.hpp"
#include "MenuUI.hpp"
#include "Player.hpp"
#include "Entity.hpp"
#include "AppUtil.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

DialogueManager::DialogueManager(std::shared_ptr<MenuUI> ui) 
    : AllObjects(nullptr, 100.0f, -1), m_ui(ui) {
    SetVisible(false);
}

void DialogueManager::StartScript(const std::string& name, std::shared_ptr<Entity> source) {
    LOG_INFO("DialogueManager: Starting script '{}'", name);
    m_source_entity = source;
    ParseScript(name);
    
    if (m_script.empty()) {
        LOG_WARN("DialogueManager: Script '{}' is empty or not found.", name);
        return;
    }

    m_mode = Mode::SCRIPT;
    m_current_line = 0;
    m_last_speaker = "";
    SetVisible(true);
    
    // Initial display
    AdvanceScript(nullptr);
}

void DialogueManager::ShowNotice(const std::string& text) {
    m_script.clear();
    m_script.push_back({-1, text, "notice", ""});
    m_mode = Mode::NOTICE;
    m_current_line = 0;
    SetVisible(true);
    
    m_ui->SetVisible(true, MenuUI::MenuType::ITEM_NOTICE);
    m_ui->SetItemNotice(text);
}

void DialogueManager::HandleInput(std::shared_ptr<Player> player) {
    if (!IsActive()) return;

    if (Util::Input::IsKeyDown(Util::Keycode::SPACE) || 
        Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
        
        if (m_mode == Mode::SCRIPT || m_mode == Mode::NOTICE) {
            AdvanceScript(player);
        }
    }
    
    // TODO: Handle selection W/S/ENTER if mode is SELECTION
}

void DialogueManager::ObjectUpdate() {
    // Current frame logic if needed (animations, etc.)
}

void DialogueManager::AdvanceScript(std::shared_ptr<Player> player) {
    if (m_mode == Mode::NOTICE) {
        m_mode = Mode::INACTIVE;
        SetVisible(false);
        m_ui->SetVisible(false);
        return;
    }

    if (m_current_line >= m_script.size()) {
        m_mode = Mode::INACTIVE;
        SetVisible(false);
        m_ui->SetVisible(false);
        return;
    }

    const auto& line = m_script[m_current_line];
    
    if (line.speaker != -1) {
        // Speaker mode (0: Player, 1: NPC)
        std::string name = (line.speaker == 0) ? "勇者" : m_last_speaker;
        m_ui->SetVisible(true, MenuUI::MenuType::ITEM_NOTICE);
        m_ui->SetItemNotice(name + ": " + line.text);
        m_current_line++;
    } else if (line.command == "end") {
        m_mode = Mode::INACTIVE;
        SetVisible(false);
        m_ui->SetVisible(false);
    } else {
        // Handle other commands (item, hide, etc.)
        if (player) ExecuteCommand(line, player);
        m_current_line++;
        AdvanceScript(player); // Recursively advance until we hit a dialogue or end
    }
}

void DialogueManager::ParseScript(const std::string& name) {
    m_script.clear();
    std::string path = std::string(MAGIC_TOWER_RESOURCE_DIR) + "/Datas/Texts/" + name + ".csv";
    auto rows = AppUtil::MapParser::ParseCsvToStrings(path);

    bool skipHeader = true;
    for (const auto& row : rows) {
        if (row.empty()) continue;
        if (skipHeader) { skipHeader = false; continue; }
        
        std::string first = row[0];
        if (first == ".") {
            m_script.push_back({-1, "", "end", ""});
            break;
        }
        
        if (first == "0" || first == "1") {
            int speakerId = std::stoi(first);
            m_script.push_back({speakerId, row.size() > 1 ? row[1] : "", "", ""});
        } else if (first == "item") {
            if (row.size() >= 3) m_script.push_back({-1, row[1], "item", row[2]});
        } else if (first == "hide") {
            m_script.push_back({-1, "", "hide", ""});
        } else if (first == "shop") {
            m_script.push_back({-1, "", "shop", ""});
        } else {
            // Fallback for older format
            m_last_speaker = first;
            m_script.push_back({1, row.size() > 1 ? row[1] : "", "", ""});
        }
    }
}

void DialogueManager::ExecuteCommand(const ScriptLine& line, std::shared_ptr<Player> player) {
    if (line.command == "item") {
        LOG_INFO("DialogueManager: Executing item command (id={})", line.extra);
        
        // Map string-based item ID to Effect
        AppUtil::Effect effect = AppUtil::Effect::NONE;
        if (line.extra == "enemy_data") effect = AppUtil::Effect::NONE; // Placeholder for mirror
        else if (line.extra == "yellow_key") effect = AppUtil::Effect::KEY_YELLOW;
        else if (line.extra == "blue_key") effect = AppUtil::Effect::KEY_BLUE;
        else if (line.extra == "red_key") effect = AppUtil::Effect::KEY_RED;
        else if (line.extra == "fly") effect = AppUtil::Effect::NONE; // TODO: Implement fly
        
        // Apply effect (default value 1 for keys if not specified)
        if (player && effect != AppUtil::Effect::NONE) {
            player->ApplyEffect(effect, 1);
        }
    } else if (line.command == "hide") {
        LOG_INFO("DialogueManager: Executing hide command");
        if (m_source_entity) {
            m_source_entity->TriggerReplacement(0); // Replace with floor
        }
    } else if (line.command == "shop") {
        LOG_INFO("DialogueManager: Executing shop command");
        // TODO: Implement shop link
    }
}
