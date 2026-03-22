#include "DialogueManager.hpp"
#include "MenuUI.hpp"
#include "Player.hpp"
#include "Entity.hpp"
#include "AppUtil.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

DialogueManager::DialogueManager(std::shared_ptr<MenuUI> ui) 
    : m_ui(ui) {

    // Initialize UI Components
    const std::string fontPath = MAGIC_TOWER_RESOURCE_DIR "/font/Cubic_11.ttf"; 

    m_background = std::make_shared<Util::GameObject>(
        std::make_unique<Util::Image>(MAGIC_TOWER_RESOURCE_DIR "/bmp/NPC/NPCDialog.bmp"), 10.0f);
    m_background->m_Transform.scale = {0.735f, 0.735f};
    m_background->m_Transform.translation = {141.0f, 150.0f}; 

    m_npc_icon = std::make_shared<Util::GameObject>(
        std::make_unique<Util::Image>(MAGIC_TOWER_RESOURCE_DIR "/bmp/NPC/elder1.bmp"), 11.0f);
    m_npc_icon->m_Transform.translation = {-28.0f, 207.0f}; // Absolute position
    m_npc_icon->m_Transform.scale = {0.735f, 0.735f}; 

    m_name_text = std::make_shared<NumericDisplayText>(fontPath, 24);
    m_name_text->SetZIndex(12.0f);
    m_name_text->m_Transform.translation = {60.0f, 205.0f}; // Absolute position
    m_name_text->SetShowNumber(false);

    m_content_text = std::make_shared<NumericDisplayText>(fontPath, 24);
    m_content_text->SetZIndex(12.0f);
    m_content_text->m_Transform.translation = {141.0f, 125.0f}; // Absolute position
    m_content_text->SetShowNumber(false);

    m_space_prompt = std::make_shared<NumericDisplayText>(fontPath, 22);
    m_space_prompt->SetZIndex(13.0f);
    m_space_prompt->m_Transform.translation = {315.0f, 65.0f}; 
    m_space_prompt->SetPrefix("-Space-");
    m_space_prompt->SetShowNumber(false);
    m_space_prompt->UpdateDisplayText();

    SetUIState(false);
}

void DialogueManager::AddToRoot(Util::Renderer& root) {
    if (m_background) root.AddChild(m_background);
    if (m_npc_icon) root.AddChild(m_npc_icon);
    if (m_name_text) root.AddChild(m_name_text);
    if (m_content_text) root.AddChild(m_content_text);
    if (m_space_prompt) root.AddChild(m_space_prompt);
}

void DialogueManager::SetVisible(bool visible) {
    SetUIState(visible);
}

void DialogueManager::SetUIState(bool dialogueVisible) {
    if (m_background) m_background->SetVisible(dialogueVisible);
    if (m_npc_icon) m_npc_icon->SetVisible(dialogueVisible);
    if (m_name_text) m_name_text->SetVisible(dialogueVisible);
    if (m_content_text) m_content_text->SetVisible(dialogueVisible);
    if (m_space_prompt) m_space_prompt->SetVisible(dialogueVisible);
}

void DialogueManager::StartScript(const std::string& name, std::shared_ptr<Entity> source) {
    LOG_INFO("DialogueManager: Starting script '{}'", name);
    m_source_entity = source;
    m_pending_notice = "";
    ParseScript(name);
    
    if (m_script.empty()) {
        LOG_WARN("DialogueManager: Script '{}' is empty or not found.", name);
        if (m_source_entity) {
            LOG_INFO("DialogueManager: No script found for NPC, triggering default hide.");
            m_source_entity->TriggerReplacement(0); // Replace with floor
        }
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
    SetUIState(false); // Ensure dialogue UI is off during simple notices
    
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

void DialogueManager::Update() {
    if (!IsActive()) return;

    // Blink Space Prompt
    if (m_space_prompt && m_mode != Mode::NOTICE) {
        static float timer = 0.0f;
        static bool visible = true;
        timer += Util::Time::GetDeltaTimeMs();
        if (timer > 500.0f) {
            visible = !visible;
            m_space_prompt->SetVisible(visible);
            timer = 0.0f;
        }
    }
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
        SetUIState(false);
        
        // If there's a pending notice (item), show it now!
        if (!m_pending_notice.empty()) {
            std::string n = m_pending_notice;
            m_pending_notice = "";
            ShowNotice(n);
        }
        return;
    }

    const auto& line = m_script[m_current_line];
    
    if (line.speaker != -1) {
        // Speaker mode (0: Player, 1: NPC)
        std::string name = "???";
        std::string iconPath = "";

        if (line.speaker == 0) {
            name = AppUtil::GetGlobalString("PlayerName", "Hero");
            iconPath = MAGIC_TOWER_RESOURCE_DIR "/bmp/Player/player_1.png";
        } else if (m_source_entity) {
            auto it = AppUtil::GlobalObjectRegistry.find(m_source_entity->GetObjectId());
            if (it != AppUtil::GlobalObjectRegistry.end() && it->second.dialog_props) {
                name = it->second.dialog_props->title;
                if (!it->second.dialog_props->icon_path.empty()) {
                    iconPath = std::string(MAGIC_TOWER_RESOURCE_DIR) + "/bmp/NPC/" + it->second.dialog_props->icon_path + ".bmp";
                }
            } else {
                name = m_last_speaker;
            }
        } else {
            name = m_last_speaker;
        }

        m_last_speaker = name; // Update for next lines

        // --- MULTI-LINE MERGE LOGIC START ---
        std::string fullText = line.text;
        int linesProcessed = 0;
        int maxLines = 3; // Commonly 3 lines in RPG boxes

        for (size_t i = m_current_line + 1; i < m_script.size(); ++i) {
            // Merge if it's the same speaker AND it's a simple speech line (no command)
            if (m_script[i].speaker == line.speaker && m_script[i].command.empty()) {
                fullText += "\n" + m_script[i].text;
                linesProcessed++;
                if (linesProcessed + 1 >= maxLines) break; // Limit reached
            } else {
                break; // Break if speaker changed or command encountered
            }
        }
        // --- MULTI-LINE MERGE LOGIC END ---

        m_name_text->SetPrefix(name);
        m_name_text->UpdateDisplayText();
        m_content_text->SetPrefix(fullText);
        m_content_text->UpdateDisplayText();
        
        if (!iconPath.empty()) {
            m_npc_icon->SetDrawable(std::make_unique<Util::Image>(iconPath));
            m_npc_icon->SetVisible(true);
        } else {
            m_npc_icon->SetVisible(false);
        }

        SetUIState(true);
        m_current_line += (linesProcessed + 1); // Skip all merged lines
    } else if (line.command == "end") {
        m_current_line = m_script.size(); // Trigger end logic in next call
        AdvanceScript(player);
    } else if (line.command == "hide") {
        if (m_source_entity) m_source_entity->TriggerReplacement(0);
        m_current_line++;
        AdvanceScript(player);
    } else if (line.command == "item") {
        if (player) ExecuteCommand(line, player);
        m_current_line++;
        AdvanceScript(player);
    } else {
        // Handle other commands (shop, etc.)
        if (player) ExecuteCommand(line, player);
        m_current_line++;
        AdvanceScript(player); 
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
        m_pending_notice = line.text; // Store text for end-of-script notice

        // Map string-based item ID to Effect
        AppUtil::Effect effect = AppUtil::Effect::NONE;
        if (line.extra == "enemy_data") effect = AppUtil::Effect::NONE; // Placeholder
        else if (line.extra == "yellow_key") effect = AppUtil::Effect::KEY_YELLOW;
        else if (line.extra == "blue_key") effect = AppUtil::Effect::KEY_BLUE;
        else if (line.extra == "red_key") effect = AppUtil::Effect::KEY_RED;
        else if (line.extra == "fly") effect = AppUtil::Effect::NONE; // TODO
        
        if (player && effect != AppUtil::Effect::NONE) {
            player->ApplyEffect(effect, 1);
        }
    } else if (line.command == "hide") {
        LOG_INFO("DialogueManager: Executing hide command");
        if (m_source_entity) {
            m_source_entity->TriggerReplacement(0);
        }
    } else if (line.command == "shop") {
        LOG_INFO("DialogueManager: Executing shop command");
        // TODO: In the future, this should probably switch to SELECTION mode
    }
}
