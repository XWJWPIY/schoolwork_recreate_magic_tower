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
    const std::string fontPath = AppUtil::GetStaticResourcePath("font/Cubic_11.ttf"); 

    m_background = std::make_shared<Util::GameObject>(
        std::make_unique<Util::Image>(AppUtil::GetStaticResourcePath("bmp/NPC/NPCDialog.bmp")), 10.0f);
    m_background->m_Transform.scale = {0.735f, 0.735f};
    m_background->m_Transform.translation = {141.0f, 150.0f}; 

    m_npc_icon = std::make_shared<Util::GameObject>(
        std::make_unique<Util::Image>(AppUtil::GetStaticResourcePath("bmp/NPC/elder1.bmp")), 11.0f);
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
        std::make_unique<Util::Image>(AppUtil::GetStaticResourcePath("bmp/Special/right_arrow_white.png")), 16.0f);
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
    m_engine.LoadScript(name);
    
    if (m_engine.GetSize() == 0) {
        LOG_WARN("DialogueManager: Script '{}' is empty or not found.", name);
        if (m_source_entity) {
            LOG_INFO("DialogueManager: No script found for NPC, triggering default hide.");
            m_source_entity->TriggerReplacement(0); // Replace with floor
        }
        return;
    }

    if (!isShop) {
        m_mode = Mode::SCRIPT;
        m_engine.Reset();
        m_last_speaker = "";
        SetVisible(true);
        
        // Initial display
        AdvanceScript(nullptr);
    }
}

void DialogueManager::ShowNotice(const std::string& text) {
    m_engine.Clear();
    m_engine.InjectStep({ScriptEngine::Speaker::SYSTEM, text, ScriptEngine::CommandType::NOTICE, ""});
    m_mode = Mode::NOTICE;
    m_engine.Reset();
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
    if (m_engine.GetSize() > 0 && m_engine.GetSteps().back().command == ScriptEngine::CommandType::END) {
        auto& steps = m_engine.GetSteps();
        auto endStep = steps.back();
        steps.pop_back();
        steps.push_back({ScriptEngine::Speaker::SYSTEM, "", ScriptEngine::CommandType::SHOP, ""});
        steps.push_back(endStep);
    } else {
        m_engine.InjectStep({ScriptEngine::Speaker::SYSTEM, "", ScriptEngine::CommandType::SHOP, ""});
        m_engine.InjectStep({ScriptEngine::Speaker::SYSTEM, "", ScriptEngine::CommandType::END, ""});
    }

    if (m_mode == Mode::INACTIVE) {
        m_mode = Mode::SCRIPT;
        m_engine.Reset();
        SetVisible(true);
        AdvanceScript(nullptr);
    }
}

void DialogueManager::ReplaceScriptText(const std::string& target, const std::string& replacement) {
    m_engine.ReplaceText(target, replacement);
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
    if (!m_is_shop_session && m_engine.HasNext()) {
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

    if (!m_engine.HasNext()) {
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

    const auto& step = m_engine.Peek();
    
    if (step.command == ScriptEngine::CommandType::NONE) {
        // Speech Step
        std::string name = "???";
        std::string iconPath = "";

        if (step.speaker == ScriptEngine::Speaker::PLAYER) {
            name = AppUtil::GetGlobalString("PlayerName", "Hero");
            iconPath = AppUtil::GetStaticResourcePath("bmp/Player/player_1.png");
        } else if (m_source_entity) {
            auto it = AppUtil::GlobalObjectRegistry.find(m_source_entity->GetObjectId());
            if (it != AppUtil::GlobalObjectRegistry.end()) {
                const auto& meta = it->second;
                name = meta.GetString(AppUtil::Attr::TITLE, m_last_speaker);
                std::string icon = meta.GetString(AppUtil::Attr::ICON);
                if (!icon.empty()) {
                    iconPath = AppUtil::GetStaticResourcePath("bmp/NPC/" + icon + ".bmp");
                }
            } else {
                name = m_last_speaker;
            }
        } else {
            name = m_last_speaker;
        }

        m_last_speaker = name;

        if (m_is_shop_session) {
            ApplyShopLayout();
            if (!m_current_shop_data.title.empty()) name = m_current_shop_data.title;
            if (!m_current_shop_data.icon_path.empty()) {
                iconPath = AppUtil::GetStaticResourcePath("bmp/Shop/" + m_current_shop_data.icon_path);
            }
        } else {
            ApplyDialogueLayout();
        }

        m_name_text->SetPrefix(name);
        m_name_text->UpdateDisplayText();
        m_content_text->SetPrefix(step.text);
        m_content_text->UpdateDisplayText();
        
        if (!iconPath.empty()) {
            m_npc_icon->SetDrawable(std::make_unique<Util::Image>(iconPath));
            m_npc_icon->SetVisible(true);
        } else {
            m_npc_icon->SetVisible(false);
        }

        SetUIState(true);
        m_engine.Next(); // Advance to next step
    } else if (step.command == ScriptEngine::CommandType::END) {
        m_engine.SetCurrentIndex(m_engine.GetSize());
        AdvanceScript(player);
    } else if (step.command == ScriptEngine::CommandType::HIDE) {
        if (m_source_entity) m_source_entity->TriggerReplacement(0);
        m_engine.Next();
        AdvanceScript(player);
    } else if (step.command == ScriptEngine::CommandType::ITEM) {
        if (player) ExecuteCommand(step, player);
        m_engine.Next();
        AdvanceScript(player);
    } else if (step.command == ScriptEngine::CommandType::SHOP) {
        ExecuteCommand(step, player);
        if (m_mode == Mode::SELECTION) {
            // If the next step is speech, use it as the shop intro text
            if (m_engine.HasNext()) {
                const auto& next = m_engine.Peek();
                if (next.command == ScriptEngine::CommandType::NONE) {
                    m_content_text->SetPrefix(next.text);
                    m_content_text->UpdateDisplayText();
                    m_content_text->SetVisible(true);
                    m_engine.Next();
                }
            }
            return;
        }
        m_engine.Next();
        AdvanceScript(player); 
    }
}

// ParseScript removed, now handled by m_engine.LoadScript

void DialogueManager::ExecuteCommand(const ScriptStep& step, std::shared_ptr<Player> player) {
    if (step.command == ScriptEngine::CommandType::ITEM) {
        LOG_INFO("DialogueManager: Executing item command (id={})", step.extra);
        m_pending_notice = step.text; // Store text for end-of-script notice

        // Map string-based item ID to Effect
        AppUtil::Effect effect = AppUtil::Effect::NONE;
        if (step.extra == "enemy_data") effect = AppUtil::Effect::NONE; // Placeholder
        else if (step.extra == "yellow_key") effect = AppUtil::Effect::KEY_YELLOW;
        else if (step.extra == "blue_key") effect = AppUtil::Effect::KEY_BLUE;
        else if (step.extra == "red_key") effect = AppUtil::Effect::KEY_RED;
        else if (step.extra == "fly") effect = AppUtil::Effect::FLY;
        
        if (player && effect != AppUtil::Effect::NONE) {
            player->ApplyEffect(effect, 1);
        }
    } else if (step.command == ScriptEngine::CommandType::HIDE) {
        LOG_INFO("DialogueManager: Executing hide command");
        if (m_source_entity) {
            m_source_entity->TriggerReplacement(0);
        }
    } else if (step.command == ScriptEngine::CommandType::SHOP) {
        // If triggered from script and no data provided, load default path
        if (m_on_selection == nullptr) {
            std::string path = m_script_name + "_option";
            std::string option_full_path = AppUtil::GetStaticResourcePath("Datas/Texts/" + path + ".csv");
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
                            AppUtil::Effect type = AppUtil::AttributeRegistry::ToEffect(eff.type_id);
                            switch (type) {
                                case AppUtil::Effect::COIN: if (player->GetAttr(AppUtil::Effect::COIN) < cost) canAfford = false; break;
                                case AppUtil::Effect::EXP: if (player->GetAttr(AppUtil::Effect::EXP) < cost) canAfford = false; break;
                                case AppUtil::Effect::HP: if (player->GetAttr(AppUtil::Effect::HP) <= cost) canAfford = false; break;
                                case AppUtil::Effect::KEY_YELLOW: if (player->GetAttr(AppUtil::Effect::KEY_YELLOW) < cost) canAfford = false; break;
                                case AppUtil::Effect::KEY_BLUE: if (player->GetAttr(AppUtil::Effect::KEY_BLUE) < cost) canAfford = false; break;
                                case AppUtil::Effect::KEY_RED: if (player->GetAttr(AppUtil::Effect::KEY_RED) < cost) canAfford = false; break;
                                default: break;
                            }
                        }
                    }
                }

                if (canAfford && player) {
                    for (const auto& eff : opt.effects) {
                        player->ApplyEffect(AppUtil::AttributeRegistry::ToEffect(eff.type_id), eff.value);
                    }
                    RefreshShopOptions(m_current_shop_data);
                }
            };
        }

        m_engine.Next();
        m_mode = Mode::SELECTION;
        m_selection = 0;
        
        m_name_text->SetPrefix(m_current_shop_data.title);
        m_name_text->UpdateDisplayText();
        
        if (!m_current_shop_data.icon_path.empty()) {
            std::string iconPath = AppUtil::GetStaticResourcePath("bmp/Shop/" + m_current_shop_data.icon_path);
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
        m_background->SetDrawable(std::make_unique<Util::Image>(AppUtil::GetStaticResourcePath("bmp/NPC/NPCDialog.bmp")));
        m_background->m_Transform.translation = {141.0f, 150.0f}; 
    }
    if (m_npc_icon) m_npc_icon->m_Transform.translation = {-28.0f, 207.0f}; 
    if (m_name_text) m_name_text->m_Transform.translation = {60.0f, 205.0f}; 
    if (m_content_text) m_content_text->m_Transform.translation = {141.0f, 125.0f}; 
}

void DialogueManager::ApplyShopLayout() {
    if (m_background) {
        m_background->SetDrawable(std::make_unique<Util::Image>(AppUtil::GetStaticResourcePath("bmp/Shop/ShopDialog.bmp")));
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
