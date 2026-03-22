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

    // Shop Options
    for (int i = 0; i < 4; ++i) { 
        auto opt = std::make_shared<NumericDisplayText>(fontPath, 24);
        opt->SetShowNumber(false);
        opt->m_Transform.translation = {141.0f, -10.0f - i * 60.0f}; 
        opt->SetZIndex(15.0f);
        m_shop_options.push_back(opt);
    }

    m_shop_selector = std::make_shared<Util::GameObject>(
        std::make_unique<Util::Image>(MAGIC_TOWER_RESOURCE_DIR "/bmp/Special/right_arrow_white.png"), 16.0f);
    m_shop_selector->m_Transform.scale = {0.5f, 0.5f};

    m_price_text = std::make_shared<NumericDisplayText>(fontPath, 24);
    m_price_text->SetZIndex(15.0f);
    m_price_text->SetShowNumber(true);

    SetUIState(false);
}

void DialogueManager::AddToRoot(Util::Renderer& root) {
    if (m_background) root.AddChild(m_background);
    if (m_npc_icon) root.AddChild(m_npc_icon);
    if (m_name_text) root.AddChild(m_name_text);
    if (m_content_text) root.AddChild(m_content_text);
    if (m_space_prompt) root.AddChild(m_space_prompt);
    
    if (m_price_text) root.AddChild(m_price_text);
    for (auto& opt : m_shop_options) root.AddChild(opt);
    if (m_shop_selector) root.AddChild(m_shop_selector);
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
    
    // Always hide shop elements initially; SELECTION mode unhides them
    for (auto& opt : m_shop_options) opt->SetVisible(false);
    if (m_shop_selector) m_shop_selector->SetVisible(false);
    if (m_price_text) m_price_text->SetVisible(false);
}

void DialogueManager::StartScript(const std::string& name, std::shared_ptr<Entity> source, bool isShop) {
    LOG_INFO("DialogueManager: Starting script '{}' (isShop={})", name, isShop);
    m_script_name = name;
    m_is_shop_session = isShop;
    m_source_entity = source;
    m_pending_notice = "";
    m_on_selection = nullptr;
    m_current_shop_data = AppUtil::ShopData(); // Reset stale data
    ParseScript(name);
    
    if (m_script.empty()) {
        LOG_WARN("DialogueManager: Script '{}' is empty or not found.", name);
        if (m_source_entity) {
            LOG_INFO("DialogueManager: No script found for NPC, triggering default hide.");
            m_source_entity->TriggerReplacement(0); // Replace with floor
        }
        return;
    }

    if (!isShop) {
        m_mode = Mode::SCRIPT;
        m_current_line = 0;
        m_last_speaker = "";
        SetVisible(true);
        
        // Initial display
        AdvanceScript(nullptr);
    }
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

void DialogueManager::StartShop(const std::string& scriptName, const AppUtil::ShopData& shopData, std::function<void(int)> onSelect, std::shared_ptr<Entity> source) {
    StartScript(scriptName, source, true);
    m_current_shop_data = shopData;
    m_on_selection = onSelect;
    
    // Inject a "shop" command at the end so it knows to enter SELECTION mode
    if (!m_script.empty() && m_script.back().command == "end") {
        auto endLine = m_script.back();
        m_script.pop_back();
        m_script.push_back({-1, "", "shop", ""});
        m_script.push_back(endLine);
    } else {
        m_script.push_back({-1, "", "shop", ""});
        m_script.push_back({-1, "", "end", ""});
    }

    if (m_mode == Mode::INACTIVE) {
        m_mode = Mode::SCRIPT;
        m_current_line = 0;
        SetVisible(true);
        AdvanceScript(nullptr);
    }
}

void DialogueManager::ReplaceScriptText(const std::string& target, const std::string& replacement) {
    for (auto& line : m_script) {
        size_t pos = line.text.find(target);
        if (pos != std::string::npos) {
            line.text.replace(pos, target.length(), replacement);
        }
    }
    // Also update existing display if it contains the target
    if (m_content_text) {
        std::string current = m_content_text->GetPrefix(); // Assuming GetPrefix() returns current text
        size_t pos = current.find(target);
        if (pos != std::string::npos) {
            current.replace(pos, target.length(), replacement);
            m_content_text->SetPrefix(current);
            m_content_text->UpdateDisplayText();
        }
    }
}

void DialogueManager::RefreshShopOptions(const AppUtil::ShopData& shopData) {
    m_current_shop_data = shopData;
    for (size_t i = 0; i < m_shop_options.size(); ++i) {
        if (i < m_current_shop_data.options.size()) {
            m_shop_options[i]->SetPrefix(m_current_shop_data.options[i].text);
            m_shop_options[i]->SetVisible(true);
            m_shop_options[i]->UpdateDisplayText();
        } else {
            m_shop_options[i]->SetVisible(false);
        }
    }

    // Dynamic Special Price update (Greed God, etc.)
    if (!shopData.special_price_str.empty()) {
        m_price_text->SetPrefix(shopData.special_price_str);
        m_price_text->SetShowNumber(false); // We pass the full string including spaces if needed
        m_price_text->UpdateDisplayText();
        m_price_text->SetVisible(true);
        // Also update text content placeholders
        ReplaceScriptText("　　　", shopData.special_price_str);
    } else {
        m_price_text->SetVisible(false);
    }
}

void DialogueManager::EndShopSelection() {
    if (!m_is_shop_session && m_current_line < m_script.size()) {
        // Continue script only if it's a regular script session (e.g. NPC with shop command)
        m_mode = Mode::SCRIPT;
        AdvanceScript(nullptr);
    } else {
        m_mode = Mode::INACTIVE;
        SetVisible(false);
        SetUIState(false);
    }
}

void DialogueManager::UpdateSelection(int index) {
    if (index >= 0 && index < static_cast<int>(m_shop_options.size())) {
        m_shop_selector->m_Transform.translation = m_shop_options[index]->m_Transform.translation + glm::vec2(-150.0f, 0.0f);
    }
}

void DialogueManager::HandleInput(std::shared_ptr<Player> player) {
    if (!IsActive()) return;

    if (m_mode == Mode::SELECTION) {
        int opt_count = static_cast<int>(m_current_shop_data.options.size());
        if (Util::Input::IsKeyDown(Util::Keycode::W) || Util::Input::IsKeyDown(Util::Keycode::UP)) {
            m_selection = (m_selection - 1 + opt_count) % opt_count;
            UpdateSelection(m_selection);
        }
        if (Util::Input::IsKeyDown(Util::Keycode::S) || Util::Input::IsKeyDown(Util::Keycode::DOWN)) {
            m_selection = (m_selection + 1) % opt_count;
            UpdateSelection(m_selection);
        }
        if (Util::Input::IsKeyDown(Util::Keycode::SPACE) || Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
            if (m_on_selection) m_on_selection(m_selection);
        }
        if (Util::Input::IsKeyDown(Util::Keycode::ESCAPE) || Util::Input::IsKeyDown(Util::Keycode::Q)) {
            int exitIdx = opt_count - 1;
            for (int i = 0; i < opt_count; ++i) {
                if (m_current_shop_data.options[i].text == "Exit") exitIdx = i;
            }
            if (m_on_selection) m_on_selection(exitIdx);
        }
        return;
    }

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
    if (m_space_prompt && m_mode != Mode::NOTICE && m_mode != Mode::SELECTION) {
        static float timer = 0.0f;
        static bool visible = true;
        timer += Util::Time::GetDeltaTimeMs();
        if (timer > 500.0f) {
            visible = !visible;
            m_space_prompt->SetVisible(visible);
            timer = 0.0f;
        }
    } else if (m_space_prompt && (m_mode == Mode::SELECTION || m_mode == Mode::NOTICE)) {
        m_space_prompt->SetVisible(false);
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

        if (m_is_shop_session) {
            ApplyShopLayout();
            if (!m_current_shop_data.title.empty()) {
                name = m_current_shop_data.title;
            }
            if (!m_current_shop_data.icon_path.empty()) {
                iconPath = std::string(MAGIC_TOWER_RESOURCE_DIR) + "/bmp/Shop/" + m_current_shop_data.icon_path;
            }
        } else {
            ApplyDialogueLayout();
        }

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
        ExecuteCommand(line, player);
        if (m_mode == Mode::SELECTION) {
            std::string shopText = "";
            size_t next_line = m_current_line;
            while (next_line < m_script.size() && m_script[next_line].speaker != -1) {
                if (!shopText.empty()) shopText += "\n";
                shopText += m_script[next_line].text;
                next_line++;
            }
            if (!shopText.empty()) {
                m_content_text->SetPrefix(shopText);
                m_content_text->UpdateDisplayText();
                m_content_text->SetVisible(true);
                m_current_line = next_line;
            }
            return;
        }
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
        // If triggered from script and no data provided, load default path
        if (m_on_selection == nullptr) {
            std::string path = m_script_name + "_option";
            std::string option_full_path = std::string(MAGIC_TOWER_RESOURCE_DIR) + "/Datas/Texts/" + path + ".csv";
            m_current_shop_data.options = AppUtil::MapParser::ParseShopOptions(option_full_path);
            if (m_current_shop_data.options.empty()) {
                m_current_shop_data.options.push_back({"No Inventory Found", {}});
            }
            m_current_shop_data.options.push_back({"Exit", {}});
            
            // For scripts, the title should be the current NPC name
            m_current_shop_data.title = m_name_text->GetPrefix(); 
            
            // For scripts, try to keep the NPC icon if possible
            // (The icon is already set in AdvanceScript before reaching 'shop')
            
            m_on_selection = [this, player](int selection) {
                if (selection < 0 || selection >= static_cast<int>(m_current_shop_data.options.size())) return;
                const auto& opt = m_current_shop_data.options[selection];
                
                if (opt.text == "Exit") {
                    m_on_selection = nullptr;
                    m_current_shop_data.options.clear();
                    EndShopSelection();
                    return;
                }
                
                // --- Simple Purchase Logic ---
                bool canAfford = true;
                if (player) {
                    for (const auto& eff : opt.effects) {
                        if (eff.value < 0) {
                            int cost = -eff.value;
                            switch (eff.type) {
                                case AppUtil::Effect::COIN: if (player->GetCoins() < cost) canAfford = false; break;
                                case AppUtil::Effect::EXP: if (player->GetExp() < cost) canAfford = false; break;
                                case AppUtil::Effect::HP: if (player->GetHp() <= cost) canAfford = false; break;
                                case AppUtil::Effect::KEY_YELLOW: if (player->GetYellowKeys() < cost) canAfford = false; break;
                                case AppUtil::Effect::KEY_BLUE: if (player->GetBlueKeys() < cost) canAfford = false; break;
                                case AppUtil::Effect::KEY_RED: if (player->GetRedKeys() < cost) canAfford = false; break;
                                default: break;
                            }
                        }
                    }
                }

                if (canAfford && player) {
                    for (const auto& eff : opt.effects) player->ApplyEffect(eff.type, eff.value);
                    RefreshShopOptions(m_current_shop_data);
                }
            };
        }

        m_current_line++;
        m_mode = Mode::SELECTION;
        m_selection = 0;
        
        m_name_text->SetPrefix(m_current_shop_data.title);
        m_name_text->UpdateDisplayText();
        
        if (!m_current_shop_data.icon_path.empty()) {
            std::string iconPath = std::string(MAGIC_TOWER_RESOURCE_DIR) + "/bmp/Shop/" + m_current_shop_data.icon_path;
            m_npc_icon->SetDrawable(std::make_unique<Util::Image>(iconPath));
            m_npc_icon->SetVisible(true);
        } else if (!m_is_shop_session) {
            // Keep the icon set by StartScript/AdvanceScript for NPC shops
            m_npc_icon->SetVisible(true);
        } else {
            m_npc_icon->SetVisible(false);
        }

        ApplyShopLayout();
        SetUIState(true); // Ensure background and texts are visible
        
        RefreshShopOptions(m_current_shop_data);
        m_shop_selector->SetVisible(true);
        UpdateSelection(m_selection);
        
        if (m_space_prompt) m_space_prompt->SetVisible(false);
    }
}

void DialogueManager::ApplyDialogueLayout() {
    if (m_background) {
        m_background->SetDrawable(std::make_unique<Util::Image>(MAGIC_TOWER_RESOURCE_DIR "/bmp/NPC/NPCDialog.bmp"));
        m_background->m_Transform.translation = {141.0f, 150.0f}; 
    }
    if (m_npc_icon) m_npc_icon->m_Transform.translation = {-28.0f, 207.0f}; 
    if (m_name_text) m_name_text->m_Transform.translation = {60.0f, 205.0f}; 
    if (m_content_text) m_content_text->m_Transform.translation = {141.0f, 125.0f}; 
}

void DialogueManager::ApplyShopLayout() {
    if (m_background) {
        m_background->SetDrawable(std::make_unique<Util::Image>(MAGIC_TOWER_RESOURCE_DIR "/bmp/Shop/ShopDialog.bmp"));
        m_background->m_Transform.translation = {141.0f, 0.0f}; 
    }
    if (m_npc_icon) m_npc_icon->m_Transform.translation = {28.0f, 141.0f}; 
    if (m_name_text) m_name_text->m_Transform.translation = {141.0f, 200.0f}; 
    if (m_content_text) {
        m_content_text->m_Transform.translation = {205.0f, 130.0f};
        m_content_text->SetVisible(true);
    }
    if (m_price_text) {
        // Position it exactly over the three full-width spaces
        m_price_text->m_Transform.translation = {155.0f, 130.0f}; 
    }
}
