#include "App.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

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

  // Factory for ThingsMap (Create specialized Entities based on ID)
  auto replacementComp = std::make_shared<DynamicReplacementComponent>(
      [this](int x, int y, int id) { this->m_things_map->SetObject(x, y, id); });

  FloorMap::ObjectFactory thingsObjFactory =
      [this, replacementComp](int id) -> std::shared_ptr<AllObjects> {
    std::shared_ptr<Entity> entity;
    if (id >= 200 && id < 300)
      entity = std::make_shared<Item>(id, [this](const std::string& text) { this->ShowItemNotice(text); });
    else if (id >= 300 && id < 400)
      entity = std::make_shared<Door>(id);
    else if (id >= 400 && id < 500)
      entity = std::make_shared<Enemy>(id);
    else if (id >= 500 && id < 600)
      entity = std::make_shared<NPC>(id);
    else if (id >= 600 && id < 700)
      entity = std::make_shared<Shop>(id);
    else if (id >= 700 && id < 800)
      entity = std::make_shared<Stair>(
          id, [this](int delta) { this->ChangeFloor(delta); });
    else {
      // Fallback or ID 0
      class UnknownEntity : public Entity {
      public:
        UnknownEntity(int i)
            : Entity(i, MAGIC_TOWER_RESOURCE_DIR "/bmp/Door/no_door.png", false) {
          if (i == 0)
            SetVisible(false);
        }
        void Reaction(std::shared_ptr<Player> player) override {
          LOG_DEBUG("Unknown entity Reaction");
        }
      };
      entity = std::make_shared<UnknownEntity>(id);
    }

    if (entity) {
      entity->SetReplacementComponent(replacementComp);
    }
    return entity;
  };

  m_road_map = std::make_shared<FloorMap>(roadObjFactory, 141.0f, 0.0f, 0.735f,
                                         0.735f, -5.0f); // Render at Z=-5
  m_road_map->SetRenderer(&m_root);
  m_road_map->AddToRenderer(); // Add default blocks to root

  for (int i = 0; i < AppUtil::TOTAL_STORY; ++i) {
    auto roadData = AppUtil::MapParser::ParseCsv(
        MAGIC_TOWER_RESOURCE_DIR "/Datas/Maps/RoadMap" + std::to_string(i) + ".csv");
    if (!roadData.empty()) {
      m_road_map->LoadFloorData(roadData, i);
    }
  }
  m_road_map->SetAllVisible(false);

  m_things_map = std::make_shared<FloorMap>(
      thingsObjFactory, 141.0f, 0.0f, 0.735f, 0.735f, -4.0f,
      m_road_map->GetBaseSize()); // Use RoadMap spacing
  m_things_map->SetRenderer(&m_root);
  m_things_map->AddToRenderer(); // Add default blocks to root

  for (int i = 0; i < AppUtil::TOTAL_STORY; ++i) {
    auto thingsData = AppUtil::MapParser::ParseCsv(MAGIC_TOWER_RESOURCE_DIR
                                                   "/Datas/Maps/ThingsMap" +
                                                   std::to_string(i) + ".csv");
    if (!thingsData.empty()) {
      m_things_map->LoadFloorData(thingsData, i);
    }
  }
  m_things_map->SetAllVisible(false);


  // Status UI Initialization
  m_status_ui = std::make_shared<StatusUI>();
  m_status_ui->AddToRoot(m_root);

  // Player Initialization
  m_player = std::make_shared<Player>();
  m_player->SyncPosition(m_road_map);
  m_root.AddChild(m_player);
  m_player->SetVisible(false);

  // Menu UI Initialization (Notice, Fly, etc.)
  m_menu_ui = std::make_shared<MenuUI>();
  m_menu_ui->AddToRoot(m_root);
}

void App::Update() {

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
      int current_frame = static_cast<int>(m_loading_timer / 150.0f) + 1; // 150ms per frame
      if (current_frame <= 4) {
        if (current_frame != m_loading_frame) {
          m_loading_frame = current_frame;
          m_background->SetLoadingFrame(m_loading_frame);
        }
      } else {
        // Animation finished
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

  case AppUtil::GameState::SHOP:
    if (Util::Input::IsKeyDown(Util::Keycode::W) || Util::Input::IsKeyDown(Util::Keycode::UP)) {
      m_shop_selection = (m_shop_selection - 1 + static_cast<int>(m_current_shop_data.options.size())) % m_current_shop_data.options.size();
      m_menu_ui->UpdateShopSelection(m_shop_selection);
    }
    if (Util::Input::IsKeyDown(Util::Keycode::S) || Util::Input::IsKeyDown(Util::Keycode::DOWN)) {
      m_shop_selection = (m_shop_selection + 1) % m_current_shop_data.options.size();
      m_menu_ui->UpdateShopSelection(m_shop_selection);
    }
    if (Util::Input::IsKeyDown(Util::Keycode::SPACE) || Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
      if (m_shop_selection >= 0 && m_shop_selection < m_current_shop_data.options.size()) {
        const auto& opt = m_current_shop_data.options[m_shop_selection];
        if (opt.text == "Exit") {
          m_game_state = AppUtil::GameState::PLAYING;
          m_menu_ui->SetVisible(false);
        } else {
          // Check affordability for all costs (negative values)
          bool canAfford = true;
          for (const auto& eff : opt.effects) {
            if (eff.value < 0) {
              if (eff.type == AppUtil::Effect::COIN && m_player->GetCoins() < -eff.value) canAfford = false;
              if (eff.type == AppUtil::Effect::EXP && m_player->GetExp() < -eff.value) canAfford = false;
              if (eff.type == AppUtil::Effect::HP && m_player->GetHp() <= -eff.value) canAfford = false;
              if (eff.type == AppUtil::Effect::KEY_YELLOW && m_player->GetYellowKeys() < -eff.value) canAfford = false;
              if (eff.type == AppUtil::Effect::KEY_BLUE && m_player->GetBlueKeys() < -eff.value) canAfford = false;
              if (eff.type == AppUtil::Effect::KEY_RED && m_player->GetRedKeys() < -eff.value) canAfford = false;
            }
          }

          if (canAfford) {
            for (const auto& eff : opt.effects) {
              m_player->ApplyEffect(eff.type, eff.value);
            }

            // Increment transaction count in registry
            auto it = AppUtil::GlobalObjectRegistry.find(m_current_shop_id);
            if (it != AppUtil::GlobalObjectRegistry.end() && it->second.shop_props) {
                it->second.shop_props->transaction_count++;
                // Re-apply scaling and update UI data
                ApplyShopScaling(m_current_shop_data, m_current_shop_id, it->second.shop_props->transaction_count);
                m_menu_ui->SetShopData(m_current_shop_data);
            }

            m_status_ui->Update(m_player, m_road_map->GetCurrentStory());
          }
        }
      }
    }
    if (Util::Input::IsKeyDown(Util::Keycode::ESCAPE) || Util::Input::IsKeyDown(Util::Keycode::Q)) {
      m_game_state = AppUtil::GameState::PLAYING;
      m_menu_ui->SetVisible(false);
    }
    break;

  case AppUtil::GameState::PLAYING:
    if (m_player->GetPendingShop() != -1) {
      int id = m_player->GetPendingShop();
      m_player->SetPendingShop(-1);

      auto it = AppUtil::GlobalObjectRegistry.find(id);
      if (it != AppUtil::GlobalObjectRegistry.end() && it->second.shop_props) {
        auto& shop = it->second.shop_props;
        
        // Don't open shop if title is None (decorative tiles 601, 603)
        if (shop->title != "None") {
            m_current_shop_data.title = shop->title;
            m_current_shop_data.icon_path = shop->icon_path;
            m_current_shop_data.transaction_count = shop->transaction_count;
            m_current_shop_data.options.clear();

            // Standardized path: Resources/Datas/Texts/[shop_name]_option.csv
            std::string shop_name = it->second.name;
            std::string full_path = std::string(MAGIC_TOWER_RESOURCE_DIR) + "/Datas/Texts/" + shop_name + "_option.csv";

            // Try to load options from CSV. If it fails, ParseShopOptions will return an empty vector.
            m_current_shop_data.options = AppUtil::MapParser::ParseShopOptions(full_path);

            if (m_current_shop_data.options.empty()) {
                // Fallback to minimal if no CSV found or empty
                m_current_shop_data.options.push_back({"No Inventory Found", {}});
            }
            m_current_shop_data.options.push_back({"Exit", {}});

            m_current_shop_id = id;
            ApplyShopScaling(m_current_shop_data, id, shop->transaction_count);

            m_game_state = AppUtil::GameState::SHOP;
            m_shop_selection = 0;
            m_menu_ui->SetVisible(true, MenuUI::MenuType::SHOP);
            m_menu_ui->SetShopData(m_current_shop_data);
            m_menu_ui->UpdateShopSelection(m_shop_selection);
        }
        break;
      }
    }

    if (m_status_ui) {
      m_status_ui->Update(m_player, m_road_map->GetCurrentStory());
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

    if (Util::Input::IsKeyDown(Util::Keycode::F)) {
      m_game_state = AppUtil::GameState::FAST_ELEVATOR;
      m_preview_floor = m_road_map->GetCurrentStory();
      m_menu_ui->SetTargetFloor(m_preview_floor);
      m_menu_ui->SetVisible(true, MenuUI::MenuType::FAST_ELEVATOR);
      LOG_INFO("Entered FAST_ELEVATOR mode at floor {}", m_preview_floor);
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
        int targetStair = (m_preview_floor < currentStory) ? 701 : 702; // Down -> Up stair, Up -> Down stair
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

  if (m_game_state == AppUtil::GameState::PLAYING || m_game_state == AppUtil::GameState::SHOP) {
    m_road_map->Update();
    m_things_map->Update();
  }

  // Handle Modal Dialogs
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

  m_root.Update();

  /*
   * Do not touch the code below as they serve the purpose for
   * closing the window.
   */

  if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
    m_current_state = STATE::END;
  }
}

void App::End() { // NOLINT(this method will mutate members in the future)
  LOG_TRACE("End");
}

void App::Restart() {
  LOG_INFO("Restarting game...");
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
      m_player->SyncPosition(m_road_map);
    }
    LOG_INFO("Teleported to story {} at stair {}", targetStory, targetStairId);
  }
}

void App::ShowItemNotice(const std::string& text) {
  if (m_menu_ui) {
    m_menu_ui->SetItemNotice(text);
    m_game_state = AppUtil::GameState::ITEM_DIALOG;
  }
}

void App::HideItemNotice() {
  if (m_menu_ui) {
    m_menu_ui->SetVisible(false); // This will clear ITEM_NOTICE
  }
}

void App::ApplyShopScaling(AppUtil::ShopData& data, int id, int times) {
    std::string shop_name = "";
    auto it = AppUtil::GlobalObjectRegistry.find(id);
    if (it != AppUtil::GlobalObjectRegistry.end()) {
        shop_name = it->second.name;
    }

    // Load dialogue from Resources/Datas/Texts/[shop_name].csv
    std::string dialogue_path = std::string(MAGIC_TOWER_RESOURCE_DIR) + "/Datas/Texts/" + shop_name + ".csv";
    auto dialogue_data = AppUtil::MapParser::ParseCsvToStrings(dialogue_path);
    data.prompts.clear();
    for (const auto& row : dialogue_data) {
        if (!row.empty() && !row[0].empty()) data.prompts.push_back(row[0]);
    }

    if (id == 612) { // War God (EXP Shop)
        return;
    }

    if (id != 602) { // Other shops
        return;
    }

    // P = 20 + times * 1 + Bonus
    int current_cost = 20 + times * 1 + (times > 25 ? (times - 25) * 4 : 0);
    
    std::string next_p_line = "";
    next_p_line += std::to_string(current_cost);
    data.prompts.push_back(next_p_line);

    for (size_t i = 0; i < data.prompts.size(); ++i) {
        std::string& prompt = data.prompts[i];
        // Find the line with placeholders (3 ideographic spaces)
        size_t pos = prompt.find("　　　");
        if (pos != std::string::npos) {
            std::string price_str = std::to_string(current_cost);
            // Better alignment Padding (half-width spaces to fit full-width area)
            if (price_str.length() == 2) price_str = " " + price_str; 
            
            prompt.replace(pos, 9, price_str); 
        }
    }

    for (auto& opt : data.options) {
        if (opt.text == "Exit") continue;

        // Update Coin cost in effects
        for (auto& eff : opt.effects) {
            if (eff.type == AppUtil::Effect::COIN) {
                eff.value = -current_cost;
            }
        }

        // Strip inline price like (20G) from option text
        size_t pos = opt.text.find("(");
        if (pos != std::string::npos) {
            opt.text = opt.text.substr(0, pos);
        }
    }
}
