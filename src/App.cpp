#include "App.hpp"
#include "AppUtil.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

#include "DialogueManager.hpp"
#include "Door.hpp"
#include "Enemy.hpp"
#include "Entity.hpp"
#include "Item.hpp"
#include "MapBlock.hpp"
#include "NPC.hpp"
#include "MenuUI.hpp"
#include "Player.hpp"
#include "Shop.hpp"
#include "Stair.hpp"
#include "StatusUI.hpp"
#include "Trigger.hpp"

void App::Start() {
  m_current_state = STATE::UPDATE;

  m_background = std::make_shared<Background>();
  m_root.AddChild(m_background);
  m_game_state = AppUtil::GameState::MAIN_MENU;
}

void App::InitializeGame() {
  LOG_TRACE("InitializeGame");

  // Factory for RoadMap (creates MapBlocks)
  FloorMap::ObjectFactory roadObjFactory =
      [](int id) -> std::shared_ptr<AllObjects> {
    return std::make_shared<MapBlock>(id);
  };

  // Factory for ThingsMap (creates specialized Entities based on ID)
  auto replacementComp = std::make_shared<DynamicReplacementComponent>(
      [this](int x, int y, int id) { this->m_things_map->SetObject(x, y, id); });

  FloorMap::ObjectFactory thingsObjFactory =
      [this, replacementComp](int id) -> std::shared_ptr<AllObjects> {
    std::shared_ptr<Entity> entity;

    if (id >= 200 && id < 300) {
      entity = std::make_shared<Item>(
          id, [this](const std::string& text) { this->ShowItemNotice(text); });
    } else if (id >= 300 && id < 400) {
      entity = std::make_shared<Door>(id);
    } else if (id >= 400 && id < 500) {
      entity = std::make_shared<Enemy>(id);
    } else if (id >= 500 && id < 600) {
      entity = std::make_shared<NPC>(
          id, [this](std::shared_ptr<NPC> npc, const std::string& path) {
              std::string scriptName = std::to_string(this->m_road_map->GetCurrentStory()) + "_" + path;
              this->m_dialogue_manager->StartScript(scriptName, npc);
          });
    } else if (id >= 600 && id < 700) {
      // Shop entity: inject open/close callbacks so it can drive state transitions
      entity = std::make_shared<Shop>(
          id,
          // onOpen: record which shop is active and switch to SHOP state
          [this](Shop& s) {
              m_active_shop = &s;
              m_game_state = AppUtil::GameState::SHOP;
          },
          // onClose: clear pointer and return to PLAYING
          [this]() {
              m_active_shop = nullptr;
              m_game_state = AppUtil::GameState::PLAYING;
          });
    } else if (id >= 700 && id < 800) {
      entity = std::make_shared<Stair>(
          id, [this](int delta) { this->ChangeFloor(delta); });
    } else if (id >= 800 && id < 900) {
      entity = std::make_shared<Trigger>(
          id, [this](std::shared_ptr<Trigger> t, const std::string& path) {
              std::string scriptName = std::to_string(this->m_road_map->GetCurrentStory()) + "_" + path;
              this->m_dialogue_manager->StartScript(scriptName, t);
          });
    } else {
      // Fallback for unknown or ID 0 (now handled by Entity itself for size templating)
      entity = std::make_shared<Entity>(id, AppUtil::GetStaticResourcePath("bmp/Road/road1.bmp"), true);
      if (id == 0) entity->SetVisible(false);
    }

    if (entity) {
      entity->SetReplacementComponent(replacementComp);
    }
    return entity;
  };

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

  // Status UI
  m_status_ui = std::make_shared<StatusUI>();
  m_status_ui->AddToRoot(m_root);

  // Player
  m_player = std::make_shared<Player>();
  m_player->SyncPosition(m_road_map);
  m_root.AddChild(m_player);
  m_player->SetVisible(false);

  // Menu UI
  m_menu_ui = std::make_shared<MenuUI>();
  m_menu_ui->AddToRoot(m_root);

  m_dialogue_manager = std::make_shared<DialogueManager>(m_menu_ui);
  m_dialogue_manager->AddToRoot(m_root);
}

void App::Update() {
  bool dialogueActive = (m_dialogue_manager && m_dialogue_manager->IsActive());
  if (dialogueActive) {
      m_dialogue_manager->HandleInput(m_player);
  }

  if (!dialogueActive) {
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
        m_menu_ui->UpdateArrows(m_road_map->GetCurrentStory());
      }
    }
    break;

  // ── SHOP: input is natively handled by DialogueManager ───────────────
  case AppUtil::GameState::SHOP:
    // Input handled by DialogueManager
    break;

  // ── PLAYING ───────────────────────────────────────────────────────────
  case AppUtil::GameState::PLAYING:
    // Open shop if player stepped onto one
    if (m_player->GetPendingShop() != -1) {
      int id = m_player->GetPendingShop();
      m_player->SetPendingShop(-1);

      // Locate the Shop entity in thingsMap and call Open()
      auto obj = m_things_map->FindFirstObjectOfId(id);
      if (auto shop = std::dynamic_pointer_cast<Shop>(obj)) {
        shop->Open(m_player, *m_dialogue_manager, m_road_map->GetCurrentStory());
        // State switch is handled by the onOpen callback inside Open()
        break;
      }
    }

    if (Util::Input::IsKeyDown(Util::Keycode::W) ||
        Util::Input::IsKeyDown(Util::Keycode::UP)) {
      m_player->Move(0, -1, m_road_map, m_things_map);
    }
    if (Util::Input::IsKeyDown(Util::Keycode::S) ||
        Util::Input::IsKeyDown(Util::Keycode::DOWN)) {
      m_player->Move(0, 1, m_road_map, m_things_map);
    }
    if (Util::Input::IsKeyDown(Util::Keycode::A) ||
        Util::Input::IsKeyDown(Util::Keycode::LEFT)) {
      m_player->Move(-1, 0, m_road_map, m_things_map);
    }
    if (Util::Input::IsKeyDown(Util::Keycode::D) ||
        Util::Input::IsKeyDown(Util::Keycode::RIGHT)) {
      m_player->Move(1, 0, m_road_map, m_things_map);
    }

    if (Util::Input::IsKeyDown(Util::Keycode::W) ||
        Util::Input::IsKeyDown(Util::Keycode::UP) ||
        Util::Input::IsKeyDown(Util::Keycode::S) ||
        Util::Input::IsKeyDown(Util::Keycode::DOWN) ||
        Util::Input::IsKeyDown(Util::Keycode::A) ||
        Util::Input::IsKeyDown(Util::Keycode::LEFT) ||
        Util::Input::IsKeyDown(Util::Keycode::D) ||
        Util::Input::IsKeyDown(Util::Keycode::RIGHT)) {
      LOG_INFO("Player Position: Floor {}, Grid({}, {})",
               m_road_map->GetCurrentStory(), m_player->GetGridX(),
               m_player->GetGridY());
    }

    if (Util::Input::IsKeyDown(Util::Keycode::NUM_8) ||
        Util::Input::IsKeyDown(Util::Keycode::KP_8)) {
      ChangeFloor(1);
    }
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_2) ||
        Util::Input::IsKeyDown(Util::Keycode::KP_2)) {
      ChangeFloor(-1);
    }

    if (Util::Input::IsKeyDown(Util::Keycode::F) && m_player->GetAttr(AppUtil::Effect::FLY) > 0) {
      m_game_state = AppUtil::GameState::FAST_ELEVATOR;
      m_preview_floor = m_road_map->GetCurrentStory();
      m_menu_ui->SetTargetFloor(m_preview_floor);
      m_menu_ui->SetVisible(true, MenuUI::MenuType::FAST_ELEVATOR);
      LOG_INFO("Entered FAST_ELEVATOR mode at floor {}", m_preview_floor);
    }

    if (Util::Input::IsKeyDown(Util::Keycode::G)) {
      m_game_state = AppUtil::GameState::FAST_ELEVATOR;
      m_preview_floor = m_road_map->GetCurrentStory();
      m_menu_ui->SetTargetFloor(m_preview_floor);
      m_menu_ui->SetVisible(true, MenuUI::MenuType::FAST_ELEVATOR);
      LOG_INFO("Entered FAST_ELEVATOR mode at floor {} (Debug)", m_preview_floor);
    }

    if (Util::Input::IsKeyDown(Util::Keycode::L)) {
      m_game_state = AppUtil::GameState::INSTRUCTIONS;
      m_menu_ui->SetVisible(true, MenuUI::MenuType::NOTICE);
      LOG_INFO("Switched to INSTRUCTIONS state");
    }

    if (Util::Input::IsKeyDown(Util::Keycode::R)) {
      Restart();
    }
    break;

  case AppUtil::GameState::INSTRUCTIONS:
    if (Util::Input::IsKeyDown(Util::Keycode::L)) {
      m_game_state = AppUtil::GameState::PLAYING;
      m_menu_ui->SetVisible(false);
      LOG_INFO("Switched to PLAYING state");
    }
    break;

  case AppUtil::GameState::FAST_ELEVATOR:
    if (Util::Input::IsKeyDown(Util::Keycode::W) || Util::Input::IsKeyDown(Util::Keycode::UP)) {
      if (m_preview_floor < AppUtil::TOTAL_STORY - 1) {
        m_preview_floor++;
        m_menu_ui->SetTargetFloor(m_preview_floor);
      }
    }
    if (Util::Input::IsKeyDown(Util::Keycode::S) || Util::Input::IsKeyDown(Util::Keycode::DOWN)) {
      if (m_preview_floor > 0) {
        m_preview_floor--;
        m_menu_ui->SetTargetFloor(m_preview_floor);
      }
    }
    if (Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
      int currentStory = m_road_map->GetCurrentStory();
      if (m_preview_floor != currentStory) {
        int targetStair = (m_preview_floor < currentStory) ? 701 : 702;
        TeleportToFloor(m_preview_floor, targetStair);
      }
      m_game_state = AppUtil::GameState::PLAYING;
      m_menu_ui->SetVisible(false);
      LOG_INFO("Confirmed floor switch to {}", m_preview_floor);
    }
    if (Util::Input::IsKeyDown(Util::Keycode::F)) {
      m_game_state = AppUtil::GameState::PLAYING;
      m_menu_ui->SetVisible(false);
      LOG_INFO("Cancelled FAST_ELEVATOR mode");
    }
    break;
    }
  }

  if (m_game_state == AppUtil::GameState::PLAYING ||
      m_game_state == AppUtil::GameState::SHOP ||
      dialogueActive) {
    m_road_map->Update();
    m_things_map->Update();
  }

  // Item Dialog
  if (m_game_state == AppUtil::GameState::ITEM_DIALOG) {
    if (Util::Input::IsKeyDown(Util::Keycode::SPACE) ||
        Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
      HideItemNotice();
      m_game_state = AppUtil::GameState::PLAYING;
    }
  }

  if (m_player) {
    m_player->ObjectUpdate();
  }

  // Unified UI Update (Sync stats in real-time)
  if (m_status_ui && m_player && (m_game_state == AppUtil::GameState::PLAYING || 
                                  m_game_state == AppUtil::GameState::SHOP ||
                                  m_game_state == AppUtil::GameState::ITEM_DIALOG)) {
    m_status_ui->Update(m_player, m_road_map->GetCurrentStory());
  }

  if (m_dialogue_manager) {
    m_dialogue_manager->Update();
  }

  m_root.Update();

  if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
    m_current_state = STATE::END;
  }
}

void App::End() { // NOLINT
  LOG_TRACE("End");
}

void App::Restart() {
  LOG_INFO("Restarting game...");
  m_active_shop = nullptr;
  m_root = Util::Renderer();
  m_game_state = AppUtil::GameState::MAIN_MENU;
  m_preview_floor = 0;
  m_item_notice_timer = 0.0f;

  m_background = std::make_shared<Background>();
  m_root.AddChild(m_background);
  m_loading_timer = 0.0f;
  m_loading_frame = 0;
}

void App::ChangeFloor(int delta) {
  int nextStory = m_road_map->GetCurrentStory() + delta;
  if (nextStory >= 0 && nextStory < AppUtil::TOTAL_STORY) {
    m_road_map->SwitchStory(nextStory);
    m_things_map->SwitchStory(nextStory);
    LOG_INFO("Switched to story {}", nextStory);
    if (m_player) {
      m_player->ResetStateAfterFloorChange();
      m_player->SyncPosition(m_road_map);
    }
    LOG_INFO("Player Position (Floor Switch): Floor {}, Grid({}, {})",
             m_road_map->GetCurrentStory(), m_player->GetGridX(),
             m_player->GetGridY());
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
  }
}

void App::ShowItemNotice(const std::string& text) {
  if (m_dialogue_manager) {
    m_dialogue_manager->ShowNotice(text);
  }
}

void App::HideItemNotice() {
  if (m_menu_ui) {
    m_menu_ui->SetVisible(false);
  }
}
