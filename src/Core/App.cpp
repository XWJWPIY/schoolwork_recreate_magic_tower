#include "Core/App.hpp"
#include "Core/AppUtil.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

#include "UI/DialogueUI.hpp"
#include "UI/FlyUI.hpp"
#include "UI/NoticeUI.hpp"
#include "Objects/Door.hpp"
#include "Objects/Enemy.hpp"
#include "Objects/Entity.hpp"
#include "Objects/Item.hpp"
#include "Objects/MapBlock.hpp"
#include "Objects/NPC.hpp"
#include "Objects/Player.hpp"
#include "UI/ItemNoticeUI.hpp"
#include "Objects/Shop.hpp"
#include "Objects/Stair.hpp"
#include "UI/StatusUI.hpp"


void App::Start() {
  m_current_state = STATE::UPDATE;

  m_background = std::make_shared<Background>();
  m_root.AddChild(m_background);
  m_game_state = AppUtil::GameState::MAIN_MENU;
}

void App::InitializeGame() {
  LOG_TRACE("InitializeGame");

  // Initialize Entity Factory with app-specific callbacks
  EntityFactory::Callbacks callbacks;
  callbacks.showItemNotice = [this](const std::string& text) { this->ShowItemNotice(text); };
  callbacks.startScript = [this](std::shared_ptr<Entity> talker, const std::string& path) {
    this->m_dialogue_ui->StartScript(path, talker);
  };
  callbacks.getCurrentStory = [this]() { return this->m_road_map->GetCurrentStory(); };
  callbacks.changeFloor = [this](int val) { this->ChangeFloor(val); };
  callbacks.setFloor = [this](int story, int x, int y) { this->SetFloor(story, x, y); };
  callbacks.openShop = [this](Shop& s) {
    m_active_shop = &s;
    m_game_state = AppUtil::GameState::SHOP;
  };
  callbacks.closeShop = [this]() {
    m_active_shop = nullptr;
    m_game_state = AppUtil::GameState::PLAYING;
  };
  callbacks.startBattle = [this](std::shared_ptr<Enemy> enemy) {
      if (m_game_state != AppUtil::GameState::PLAYING) return;
      m_game_state = AppUtil::GameState::BATTLE;
      m_battle_ui->Start(m_player, enemy, [this, enemy](bool defeated) {
          if (defeated) {
              enemy->OnDefeated(m_player);
          }
          m_game_state = AppUtil::GameState::PLAYING;
      });
  };
  callbacks.loadRewardLayer = [this](const std::string& path) {
      if (this->m_things_map) {
          this->m_things_map->LoadOverlay(path);
      }
  };

  m_entity_factory = std::make_unique<EntityFactory>(callbacks);

  // Factory for RoadMap (creates MapBlocks)
  FloorMap::ObjectFactory roadObjFactory = [this](int id) {
    return m_entity_factory->CreateEntity(id);
  };

  // Factory for ThingsMap (creates specialized Entities based on ID)
  FloorMap::ObjectFactory thingsObjFactory = [this](int id) {
    return m_entity_factory->CreateEntity(id);
  };

  // Define replacement behavior
  auto replacementComp = std::make_shared<DynamicReplacementComponent>(
      [this](int x, int y, int id) { this->m_things_map->SetObject(x, y, id); },
      [this](int id) { return this->m_things_map->FindFirstObjectOfId(id, this->m_road_map->GetCurrentStory()); }
  );
  m_entity_factory->SetReplacementComponent(replacementComp);

  m_road_map = std::make_shared<FloorMap>(roadObjFactory, 141.0f, 0.0f, 0.735f,
                                         0.735f, -5.0f);
  m_road_map->SetRenderer(&m_root);
  m_road_map->LoadAllFloors("Datas/Maps/RoadMap");
  m_road_map->AddToRenderer();
  m_road_map->SetAllVisible(false);

  m_things_map = std::make_shared<FloorMap>(
      thingsObjFactory, 141.0f, 0.0f, 0.735f, 0.735f, -4.0f,
      m_road_map->GetBaseSize());
  m_things_map->SetRenderer(&m_root);
  m_things_map->LoadAllFloors("Datas/Maps/ThingsMap");
  m_things_map->AddToRenderer();
  m_things_map->SetAllVisible(false);

  // Player
  m_player = std::make_shared<Player>();
  m_player->SyncPosition(m_road_map);
  m_root.AddChild(m_player);
  m_player->SetVisible(false);

  // Status UI
  m_status_ui = std::make_shared<StatusUI>(m_player, m_road_map, 36);
  m_status_ui->AddToRoot(m_root);

  // Dialogue & Notice UI
  m_item_notice_ui = std::make_shared<ItemNoticeUI>();
  m_item_notice_ui->AddToRoot(m_root);

  m_dialogue_ui = std::make_shared<DialogueUI>(m_item_notice_ui);
  m_dialogue_ui->SetPlayer(m_player);
  m_dialogue_ui->SetOnSwitchObject([this](std::shared_ptr<Entity> source, int targetId) {
      if (source && m_things_map) {
          glm::ivec2 pos = source->GetGridPosition();
          m_things_map->SetObject(pos.x, pos.y, targetId);
          LOG_INFO("App: Switched object at ({}, {}) to ID {}", pos.x, pos.y, targetId);
      }
  });
  m_dialogue_ui->AddToRoot(m_root);

  m_fly_ui = std::make_shared<FlyUI>();
  m_fly_ui->SetPlayer(m_player);
  m_fly_ui->AddToRoot(m_root);

  m_notice_ui = std::make_shared<NoticeUI>();
  m_notice_ui->AddToRoot(m_root);

  m_enemy_book_ui = std::make_shared<EnemyBookUI>(m_player, m_things_map);
  m_enemy_book_ui->AddToRoot(m_root);

  // Register all UI Components for unified update loop
  m_ui_components.push_back(m_status_ui);
  m_ui_components.push_back(m_fly_ui);
  m_ui_components.push_back(m_notice_ui);
  m_ui_components.push_back(m_enemy_book_ui);
  m_ui_components.push_back(m_item_notice_ui);
  m_ui_components.push_back(m_dialogue_ui);

  m_battle_ui = std::make_shared<BattleUI>(AppUtil::GetStaticResourcePath("Font/Cubic_11.ttf"));
  m_battle_ui->AddToRoot(m_root);
  m_ui_components.push_back(m_battle_ui);

  m_end_scene_ui = std::make_shared<EndSceneUI>(AppUtil::GetStaticResourcePath("font/Cubic_11.ttf"));
  m_end_scene_ui->AddToRoot(m_root);
  m_ui_components.push_back(m_end_scene_ui);

  m_dialogue_ui->SetOnWin([this]() {
      m_game_state = AppUtil::GameState::WIN;
      m_end_scene_ui->Show(true);
  });
}

void App::Update() {
  // 0. Unified UI update: run all active UI components
  if (m_game_state != AppUtil::GameState::MAIN_MENU && 
      m_game_state != AppUtil::GameState::LOADING) {
    for (auto& ui : m_ui_components) {
      if (ui->IsActive()) ui->run();
    }
  }

  // 1. Main State Machine (Mode-First)
  switch (m_game_state) {
  case AppUtil::GameState::MAIN_MENU:
    if (Util::Input::IsKeyDown(Util::Keycode::SPACE)) {
      m_game_state = AppUtil::GameState::LOADING;
      m_loading_timer = 0.0f;
      m_loading_frame = 1;
      m_background->SetLoadingFrame(m_loading_frame);
    }
    break;

  case AppUtil::GameState::LOADING:
    m_loading_timer += Util::Time::GetDeltaTimeMs();
    {
      int current_frame = static_cast<int>(m_loading_timer / 150.0f) + 1;
      if (current_frame <= 4) {
        if (current_frame != m_loading_frame) {
          m_loading_frame = current_frame;
          m_background->SetLoadingFrame(m_loading_frame);
        }
      } else {
        AppUtil::RegistryLoader::LoadAllData();
        InitializeGame();
        m_game_state = AppUtil::GameState::PLAYING;
        m_background->NextPhase(1);
        m_road_map->SetAllVisible(true);
        m_things_map->SetAllVisible(true);
        m_status_ui->SetVisible(true);
        m_player->SetVisible(true);
        m_loading_timer = 0.0f;
        m_loading_frame = 0;
      }
    }
    break;

  case AppUtil::GameState::PLAYING:
    if (m_player->GetAttr(AppUtil::Effect::HP) <= 0) {
        m_game_state = AppUtil::GameState::GAME_OVER;
        m_end_scene_ui->Show(false);
        break;
    }

    // Check if a dialogue or modal UI is intercepting the logic phase
    {
        bool isIntercepted = false;
        for (auto& ui : m_ui_components) {
            if (ui->IsActive() && ui->IsIntercepting()) isIntercepted = true;
        }
        if (isIntercepted) break; // Skip map parsing if dialogue is busy
    }

    // ── Input: Shop Check ───────────────────────────────────────────
    if (m_player->GetPendingShop() != -1) {
      int id = m_player->GetPendingShop();
      m_player->SetPendingShop(-1);
      auto obj = m_things_map->FindFirstObjectOfId(id);
      if (auto shop = std::dynamic_pointer_cast<Shop>(obj)) {
        ShopUIAdapter adapter{
          [this](const std::string& name, const AppUtil::ShopData& data, std::function<void(int)> onSelect, std::shared_ptr<Entity> src) {
            m_dialogue_ui->StartShop(name, data, onSelect, src);
          },
          [this](const AppUtil::ShopData& data) {
            m_dialogue_ui->RefreshShopOptions(data);
          },
          [this]() {
            m_dialogue_ui->EndShopSelection();
          }
        };
        shop->Open(m_player, adapter, m_road_map->GetCurrentStory());
        break; 
      }
    }

    // ── Input: Toggle Overlays ──────────────────────────────────────
    if (Util::Input::IsKeyDown(Util::Keycode::D)) {
      if (m_player->GetAttr(AppUtil::Effect::ENEMY_BOOK) > 0) {
          m_enemy_book_ui->SetVisible(true);
          m_game_state = AppUtil::GameState::ENEMY_BOOK;
          LOG_INFO("[Toggle] Pressed D: Opening EnemyBookUI");
      } else {
          LOG_INFO("[Toggle] Pressed D: Heart Mirror not unlocked yet.");
      }
      break;
    }
    if (Util::Input::IsKeyDown(Util::Keycode::L)) {
      m_notice_ui->SetVisible(true);
      m_game_state = AppUtil::GameState::INSTRUCTIONS;
      LOG_INFO("[Toggle] Pressed L: Opening NoticeUI");
      break;
    }
    if (Util::Input::IsKeyDown(Util::Keycode::F)) {
      // Restriction: Floors 21-25 are disabled in normal mode
      if (!m_player->IsSuperMode() && m_road_map->GetCurrentStory() > 20) {
          LOG_INFO("[Toggle] Pressed F: Elevator disabled on floors 21-25 in normal mode.");
          break;
      }

      if (m_player->HasFly()) {
        m_game_state = AppUtil::GameState::FAST_ELEVATOR;
        m_fly_ui->Start(m_road_map->GetCurrentStory(), [this](int floor, int) {
          this->m_game_state = AppUtil::GameState::PLAYING;
          int currentStory = m_road_map->GetCurrentStory();
          if (floor != currentStory) {
            int targetStair = (floor < currentStory) ? 701 : 702;
            TeleportToFloor(floor, targetStair);
          }
        });
        break;
      }
    }
    // ── Input: Debug Cheat Mode ─────────────────────────────────────
    if (Util::Input::IsKeyDown(Util::Keycode::G)) {
        m_player->ToggleSuperMode();
        m_player->SyncPosition(m_road_map);
        break;
    }

    // ── Input: Movement ─────────────────────────────────────────────
    if (Util::Input::IsKeyDown(Util::Keycode::UP)) m_player->Move(0, -1, m_road_map, m_things_map);
    else if (Util::Input::IsKeyDown(Util::Keycode::DOWN)) m_player->Move(0, 1, m_road_map, m_things_map);
    else if (Util::Input::IsKeyDown(Util::Keycode::LEFT)) m_player->Move(-1, 0, m_road_map, m_things_map);
    else if (Util::Input::IsKeyDown(Util::Keycode::RIGHT)) m_player->Move(1, 0, m_road_map, m_things_map);

    // ── Input: Floor Switch ─────────────────────────────────────────
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_8) || Util::Input::IsKeyDown(Util::Keycode::KP_8)) ChangeFloor(1);
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_2) || Util::Input::IsKeyDown(Util::Keycode::KP_2)) ChangeFloor(-1);
    if (Util::Input::IsKeyDown(Util::Keycode::R)) Restart();

    // ─ World Update ──────────────────────────────────────────────────
    m_road_map->Update();
    m_things_map->Update();
    m_player->ObjectUpdate();
    break;

  case AppUtil::GameState::INSTRUCTIONS:
    
    if (Util::Input::IsKeyDown(Util::Keycode::L)) {
      m_notice_ui->SetVisible(false);
      LOG_INFO("[Toggle] Pressed L: Closing NoticeUI");
    }
    if (!m_notice_ui->IsActive() && !Util::Input::IsKeyDown(Util::Keycode::L)) {
      m_game_state = AppUtil::GameState::PLAYING;
    }
    break;

  case AppUtil::GameState::FAST_ELEVATOR:

    if (Util::Input::IsKeyDown(Util::Keycode::F)) {
      m_fly_ui->SetVisible(false);
    }
    if (!m_fly_ui->IsActive() && !Util::Input::IsKeyDown(Util::Keycode::F)) {
      m_game_state = AppUtil::GameState::PLAYING;
    }
    break;

  case AppUtil::GameState::SHOP:
    break;

  case AppUtil::GameState::ENEMY_BOOK:

    if (Util::Input::IsKeyDown(Util::Keycode::D)) {
      m_enemy_book_ui->SetVisible(false);
      LOG_INFO("[Toggle] Pressed D: Closing EnemyBookUI");
    }
    if (!m_enemy_book_ui->IsActive() && !Util::Input::IsKeyDown(Util::Keycode::D)) {
      m_game_state = AppUtil::GameState::PLAYING;
    }
    break;

  case AppUtil::GameState::BATTLE:
    break; // Handled by BattleUI::run()

  case AppUtil::GameState::GAME_OVER:
  case AppUtil::GameState::WIN:
    if (Util::Input::IsKeyDown(Util::Keycode::SPACE) && m_end_scene_ui->CanRestart()) {
        Restart();
    }
    break;
  }

  // 2. Render phase
  m_root.Update();

  // 3. Global Terminate Check
  if (Util::Input::IsKeyDown(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
    m_current_state = STATE::END; 
  }
}


void App::End() { // NOLINT
  LOG_TRACE("End");
}

void App::Restart() {
  LOG_INFO("Restarting game...");
  m_active_shop = nullptr;
  m_ui_components.clear(); // Clear old UI components
  m_root = Util::Renderer();
  m_game_state = AppUtil::GameState::MAIN_MENU;
  m_item_notice_timer = 0.0f;

  m_background = std::make_shared<Background>();
  m_root.AddChild(m_background);
  m_loading_timer = 0.0f;
  m_loading_frame = 0;
}

void App::ChangeFloor(int delta) {
  SetFloor(m_road_map->GetCurrentStory() + delta);
}

void App::SetFloor(int nextStory, int x, int y) {
  if (nextStory >= 0 && nextStory < AppUtil::TOTAL_STORY) {
    m_road_map->SwitchStory(nextStory);
    m_things_map->SwitchStory(nextStory);
    LOG_INFO("App: Switched to story {}", nextStory);

    if (m_player) {
      m_player->ResetStateAfterFloorChange(); 
      if (x >= 0 && y >= 0) {
        LOG_INFO("App: Portal Teleporting to ({}, {})", x, y);
        m_player->SetGridPosition(x, y);
      } else {
        LOG_INFO("App: Standard Stair Teleporting (Auto-sync)");
      }
      m_player->SyncPosition(m_road_map);
    }

    LOG_INFO("App: Final Player Position: Floor {}, Grid({}, {})",
             m_road_map->GetCurrentStory(), m_player->GetGridX(),
             m_player->GetGridY());

    UpdateHighestFloor();
  }
}

void App::UpdateHighestFloor() {
    if (!m_player || !m_road_map) return;
    int current = m_road_map->GetCurrentStory();
    if (current > m_player->GetAttr(AppUtil::Effect::HIGHEST_FLOOR)) {
        m_player->SetAttr(AppUtil::Effect::HIGHEST_FLOOR, current);
        LOG_INFO("App: Highest floor updated to {}", current);
    }
}

void App::TeleportToFloor(int targetStory, int targetStairId) {
  if (targetStory >= 0 && targetStory < AppUtil::TOTAL_STORY) {
    m_road_map->SwitchStory(targetStory);
    m_things_map->SwitchStory(targetStory);

    if (m_player) {
      glm::ivec2 pos = m_things_map->FindFirstObjectPosition(targetStairId, targetStory);
      if (pos.x != -1) {
        m_player->SetGridPosition(pos.x, pos.y);
      }
      m_player->ResetStateAfterFloorChange();
      m_player->SyncPosition(m_road_map);
    }
    LOG_INFO("Teleported to story {} at stair {}", targetStory, targetStairId);
    UpdateHighestFloor();
  }
}

void App::ShowItemNotice(const std::string& text) {
  if (m_item_notice_ui) {
    m_item_notice_ui->Show(text);
  }
}

void App::HideItemNotice() {
  if (m_item_notice_ui) {
    m_item_notice_ui->SetVisible(false);
  }
}
