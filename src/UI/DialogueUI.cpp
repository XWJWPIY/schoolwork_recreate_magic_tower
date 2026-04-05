#include "UI/DialogueUI.hpp"
#include "UI/ItemNoticeUI.hpp"
#include "Objects/Player.hpp"
#include "Core/AppUtil.hpp"
#include "Systems/ScriptEngine.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

DialogueUI::DialogueUI(std::shared_ptr<ItemNoticeUI> ui) 
    : m_ui(ui) {

    // Initialize UI Components
    const std::string fontPath = AppUtil::GetStaticResourcePath("font/Cubic_11.ttf"); 

    m_background = std::make_shared<Util::GameObject>(
        std::make_unique<Util::Image>(AppUtil::GetStaticResourcePath("bmp/NPC/NPCDialog.bmp")), 10.0f);
    m_background->m_Transform.scale = {0.735f, 0.735f};
    m_background->m_Transform.translation = {141.0f, 150.0f}; 

    m_npc_icon = std::make_shared<Util::GameObject>(
        std::make_unique<Util::Image>(AppUtil::GetStaticResourcePath("bmp/NPC/elder1.bmp")), 11.0f);
    m_npc_icon->m_Transform.translation = {-28.0f, 207.0f}; 
    m_npc_icon->m_Transform.scale = {0.735f, 0.735f}; 

    m_name_text = std::make_shared<NumericDisplayText>(fontPath, 24);
    m_name_text->SetZIndex(12.0f);
    m_name_text->m_Transform.translation = {60.0f, 205.0f}; 
    m_name_text->SetShowNumber(false);

    m_content_text = std::make_shared<NumericDisplayText>(fontPath, 24);
    m_content_text->SetZIndex(12.0f);
    m_content_text->m_Transform.translation = {141.0f, 125.0f}; 
    m_content_text->SetShowNumber(false);

    m_space_prompt = std::make_shared<NumericDisplayText>(fontPath, 22);
    m_space_prompt->SetZIndex(13.0f);
    m_space_prompt->m_Transform.translation = {315.0f, 65.0f}; 
    m_space_prompt->SetPrefix("-Space-");
    m_space_prompt->SetShowNumber(false);
    m_space_prompt->UpdateDisplayText();

    // Shop UI
    m_shop_ui = std::make_unique<ShopUI>(fontPath);

    SetUIState(false);
}

void DialogueUI::AddToRoot(Util::Renderer& root) {
    if (m_background) root.AddChild(m_background);
    if (m_npc_icon) root.AddChild(m_npc_icon);
    if (m_name_text) root.AddChild(m_name_text);
    if (m_content_text) root.AddChild(m_content_text);
    if (m_space_prompt) root.AddChild(m_space_prompt);
    
    if (m_shop_ui) m_shop_ui->AddToRoot(root);
}

void DialogueUI::SetVisible(bool visible) {
    SetUIState(visible);
}

void DialogueUI::SetUIState(bool dialogueVisible) {
    if (m_background) m_background->SetVisible(dialogueVisible);
    if (m_npc_icon) m_npc_icon->SetVisible(dialogueVisible);
    if (m_name_text) m_name_text->SetVisible(dialogueVisible);
    if (m_content_text) m_content_text->SetVisible(dialogueVisible);
    if (m_space_prompt) m_space_prompt->SetVisible(dialogueVisible);
    
    if (m_shop_ui) {
        m_shop_ui->SetVisible(dialogueVisible && m_mode == Mode::SELECTION);
    }
}

void DialogueUI::StartScript(const std::string& name, std::shared_ptr<Entity> source, bool isShop) {
    LOG_INFO("DialogueUI: Starting script '{}' (isShop={})", name, isShop);
    m_script_name = name;
    m_is_shop_session = isShop;
    m_source_entity = source;
    m_pending_notice = "";
    m_on_selection = nullptr;
    m_current_shop_data = AppUtil::ShopData(); 
    m_engine.LoadScript(name);
    
    if (m_engine.GetSize() == 0) {
        LOG_WARN("DialogueUI: Script '{}' is empty or not found.", name);
        if (m_source_entity) {
            m_source_entity->TriggerReplacement(0); 
        }
        return;
    }

    if (!isShop) {
        m_mode = Mode::SCRIPT;
        m_engine.Reset();
        m_last_speaker = "";
        SetVisible(true);
        AdvanceScript();
    }
}

void DialogueUI::ShowNotice(const std::string& text) {
    // We keep m_mode as NOTICE so run() knows to hide DialogueUI elements
    // and let ItemNoticeUI take over the screen.
    m_mode = Mode::NOTICE;
    SetVisible(true);
    SetUIState(false); 
    
    m_ui->Show(text);
}

void DialogueUI::StartShop(const std::string& scriptName, const AppUtil::ShopData& shopData, std::function<void(int)> onSelect, std::shared_ptr<Entity> source) {
    StartScript(scriptName, source, true);
    m_current_shop_data = shopData;
    m_on_selection = onSelect;
    
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
        AdvanceScript();
    }
}

void DialogueUI::ReplaceScriptText(const std::string& target, const std::string& replacement) {
    m_engine.ReplaceText(target, replacement);
    if (m_content_text) {
        std::string current = m_content_text->GetPrefix(); 
        size_t pos = current.find(target);
        if (pos != std::string::npos) {
            current.replace(pos, target.length(), replacement);
            m_content_text->SetPrefix(current);
            m_content_text->UpdateDisplayText();
        }
    }
}

void DialogueUI::RefreshShopOptions(const AppUtil::ShopData& shopData) {
    m_current_shop_data = shopData;
    if (m_shop_ui) m_shop_ui->Refresh(m_current_shop_data);
    
    if (!shopData.special_price_str.empty()) {
        ReplaceScriptText("　　　", shopData.special_price_str);
    }
}

void DialogueUI::EndShopSelection() {
    m_on_selection = nullptr;
    if (m_shop_ui) m_shop_ui->SetVisible(false);

    if (!m_is_shop_session && m_engine.HasNext()) {
        m_mode = Mode::SCRIPT;
        AdvanceScript();
    } else {
        m_mode = Mode::INACTIVE;
        SetVisible(false);
        SetUIState(false);
    }
}

void DialogueUI::HandleInput() {
    if (!IsActive()) return;

    if (m_mode == Mode::SELECTION) {
        if (m_shop_ui) m_shop_ui->run();
        return;
    }

    if (Util::Input::IsKeyDown(Util::Keycode::SPACE) || 
        Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
        
        if (m_mode == Mode::SCRIPT || m_mode == Mode::NOTICE) {
            AdvanceScript();
        }
    }
}

void DialogueUI::run() {
    if (!IsActive()) return;

    HandleInput();

    if (!IsActive()) return;

    if (m_space_prompt && m_mode != Mode::NOTICE && m_mode != Mode::SELECTION) {
        m_blink_timer += Util::Time::GetDeltaTimeMs();
        if (m_blink_timer > 1000.0f) m_blink_timer -= 1000.0f;
        m_space_prompt->SetVisible(m_blink_timer < 500.0f);
    } else if (m_space_prompt && (m_mode == Mode::SELECTION || m_mode == Mode::NOTICE)) {
        m_space_prompt->SetVisible(false);
    }
}

void DialogueUI::AdvanceScript() {
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
        
        if (!m_pending_notice.empty()) {
            std::string n = m_pending_notice;
            m_pending_notice = "";
            ShowNotice(n);
        }
        return;
    }

    const auto& step = m_engine.Peek();
    
    if (step.command == ScriptEngine::CommandType::NONE) {
        std::string name = "???";
        std::string iconPath = "";

        if (step.speaker == ScriptEngine::Speaker::PLAYER) {
            name = AppUtil::GetGlobalString("PlayerName", "Hero");
            if (m_player && m_player->IsSuperMode()) {
                iconPath = AppUtil::GetStaticResourcePath(AppUtil::Skin::SUPER_MODE_PATH);
                m_npc_icon->m_Transform.scale = {0.735f * AppUtil::Skin::SUPER_MODE_RATIO, 0.735f * AppUtil::Skin::SUPER_MODE_RATIO};
            } else {
                iconPath = AppUtil::GetStaticResourcePath("bmp/Player/player_1.png");
                m_npc_icon->m_Transform.scale = {0.735f, 0.735f};
            }
        } else if (m_source_entity) {
            m_npc_icon->m_Transform.scale = {0.735f, 0.735f}; // Reset for NPCs
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
        m_engine.Next(); 
    } else if (step.command == ScriptEngine::CommandType::END) {
        m_engine.SetCurrentIndex(m_engine.GetSize());
        AdvanceScript();
    } else if (step.command == ScriptEngine::CommandType::HIDE) {
        if (m_source_entity) m_source_entity->TriggerReplacement(0);
        m_engine.Next();
        AdvanceScript();
    } else if (step.command == ScriptEngine::CommandType::ITEM) {
        ExecuteCommand(step);
        m_engine.Next();
        AdvanceScript();
    } else if (step.command == ScriptEngine::CommandType::SWITCH) {
        ExecuteCommand(step);
        m_engine.Next();
        AdvanceScript();
    } else if (step.command == ScriptEngine::CommandType::SHOP) {
        ExecuteCommand(step);
        if (m_mode == Mode::SELECTION) {
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
        AdvanceScript(); 
    }
    else if (step.command == ScriptEngine::CommandType::WIN) {
        if (m_on_win) m_on_win();
        m_engine.Next();
        AdvanceScript();
    }
}

void DialogueUI::ExecuteCommand(const ScriptStep& step) {
    if (step.command == ScriptEngine::CommandType::ITEM) {
        m_pending_notice = step.text; 

        if (m_player) {
            // Get attribute ID from registry (this handles "enemy_book", "fly", etc. dynamically)
            int attrId = AppUtil::AttributeRegistry::GetId(step.extra);
            AppUtil::Effect effect = AppUtil::AttributeRegistry::ToEffect(attrId);

            if (effect != AppUtil::Effect::NONE) {
                m_player->ApplyEffect(effect, 1);
                LOG_INFO("DialogueUI: Applied effect {} (+1) to player", step.extra);
            } else {
                LOG_WARN("DialogueUI: Unknown attribute reward '{}' in script.", step.extra);
            }
        }
        if (m_source_entity) {
            m_source_entity->TriggerReplacement(0);
        }
    } else if (step.command == ScriptEngine::CommandType::SWITCH) {
        if (m_on_switch_object && m_source_entity) {
            int targetId = 0;
            try {
                targetId = std::stoi(step.extra);
            } catch (...) {
                LOG_ERROR("DialogueUI: Invalid target ID '{}' for switch_to command.", step.extra);
            }
            if (targetId > 0) {
                m_on_switch_object(m_source_entity, targetId);
            }
        }
    } else if (step.command == ScriptEngine::CommandType::SHOP) {
        if (m_on_selection == nullptr) {
            m_current_shop_data = ShopSystem::LoadFromStaticFile(m_script_name + "_option");
            m_current_shop_data.title = m_name_text->GetPrefix(); 
            
            if (!step.extra.empty()) {
                try { m_current_shop_data.max_transactions = std::stoi(step.extra); }
                catch (...) { LOG_ERROR("DialogueUI: Invalid max_transactions '{}'", step.extra); }
            }

            int transCount = GetTransactionCount();

            if (m_current_shop_data.max_transactions != -1 && transCount >= m_current_shop_data.max_transactions) {
                m_engine.Next();
                return; // Skip opening the shop since limit reached
            }

            m_on_selection = [this](int selection) {
                this->HandleNPCShopSelection(selection);
            };
        }

        m_engine.Next();
        m_mode = Mode::SELECTION;
        
        if (m_shop_ui) m_shop_ui->Start(m_current_shop_data, m_on_selection);

        m_name_text->SetPrefix(m_current_shop_data.title);
        m_name_text->UpdateDisplayText();
        
        if (!m_current_shop_data.icon_path.empty()) {
            std::string iconPath = AppUtil::GetStaticResourcePath("bmp/Shop/" + m_current_shop_data.icon_path);
            m_npc_icon->SetDrawable(std::make_unique<Util::Image>(iconPath));
            m_npc_icon->SetVisible(true);
        } else if (!m_is_shop_session) {
            m_npc_icon->SetVisible(true);
        } else {
            m_npc_icon->SetVisible(false);
        }

        ApplyShopLayout();
        SetUIState(true); 
        RefreshShopOptions(m_current_shop_data);
        
        if (m_space_prompt) m_space_prompt->SetVisible(false);
    }
}

void DialogueUI::ApplyDialogueLayout() {
    if (m_background) {
        m_background->SetDrawable(std::make_unique<Util::Image>(AppUtil::GetStaticResourcePath("bmp/NPC/NPCDialog.bmp")));
        m_background->m_Transform.translation = {141.0f, 150.0f}; 
    }
    if (m_npc_icon) m_npc_icon->m_Transform.translation = {-28.0f, 207.0f}; 
    if (m_name_text) m_name_text->m_Transform.translation = {60.0f, 205.0f}; 
    if (m_content_text) m_content_text->m_Transform.translation = {141.0f, 125.0f}; 
}

void DialogueUI::ApplyShopLayout() {
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
}

int DialogueUI::GetTransactionCount() const {
    if (!m_source_entity) return 0;
    auto it = AppUtil::GlobalObjectRegistry.find(m_source_entity->GetObjectId());
    if (it == AppUtil::GlobalObjectRegistry.end()) return 0;
    return it->second.GetInt(AppUtil::Attr::TRANSACTIONS);
}

int DialogueUI::IncrementTransactionCount() {
    if (!m_source_entity) return 0;
    auto it = AppUtil::GlobalObjectRegistry.find(m_source_entity->GetObjectId());
    if (it == AppUtil::GlobalObjectRegistry.end()) return 0;
    int count = it->second.GetInt(AppUtil::Attr::TRANSACTIONS) + 1;
    it->second.attributes[AppUtil::AttributeRegistry::GetId(AppUtil::Attr::TRANSACTIONS)]
        = std::to_string(count);
    return count;
}

void DialogueUI::HandleNPCShopSelection(int selection) {
    if (selection < 0 || selection >= static_cast<int>(m_current_shop_data.options.size())) return;
    const auto& opt = m_current_shop_data.options[selection];
    
    if (opt.text == "Exit") {
        m_on_selection = nullptr;
        m_current_shop_data.options.clear();
        EndShopSelection();
        return;
    }
    
    bool canAfford = true;
    if (m_player) {
        for (const auto& eff : opt.effects) {
            if (eff.value < 0) {
                int cost = -eff.value;
                AppUtil::Effect type = AppUtil::AttributeRegistry::ToEffect(eff.type_id);
                if (!m_player->MeetsRequirement(type, cost)) {
                    canAfford = false;
                    break;
                }
            }
        }
    }

    if (canAfford && m_player) {
        for (const auto& eff : opt.effects) {
            m_player->ApplyEffect(AppUtil::AttributeRegistry::ToEffect(eff.type_id), eff.value);
        }
        int newCount = IncrementTransactionCount();

        if (m_current_shop_data.max_transactions != -1 && newCount >= m_current_shop_data.max_transactions) {
            m_on_selection = nullptr;
            m_current_shop_data.options.clear();
            EndShopSelection();
        } else {
            RefreshShopOptions(m_current_shop_data);
        }
    }
}
