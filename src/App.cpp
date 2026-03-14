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
#include "Player.hpp"
#include "Shop.hpp"
#include "Stair.hpp"

void App::Start() {
  LOG_TRACE("Start");
  m_CurrentState = State::UPDATE;

  m_Background = std::make_shared<Background>();
  m_Root.AddChild(m_Background);

  // Factory for RoadMap (creates MapBlocks)
  FloorMap::BlockFactory roadFactory =
      [](int id) -> std::shared_ptr<AllObjects> {
    return std::make_shared<MapBlock>(id);
  };

  // Factory for ThingsMap (Create specialized Entities based on ID)
  auto replacementComp = std::make_shared<DynamicReplacementComponent>(
      [this](int x, int y, int id) { this->m_ThingsMap->SetBlock(x, y, id); });

  FloorMap::BlockFactory thingsFactory =
      [this, replacementComp](int id) -> std::shared_ptr<AllObjects> {
    std::shared_ptr<Entity> entity;
    if (id >= 200 && id < 300)
      entity = std::make_shared<Item>(id);
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
        void reaction(std::shared_ptr<Player> player) override {
          LOG_DEBUG("Unknown entity reaction");
        }
      };
      entity = std::make_shared<UnknownEntity>(id);
    }

    if (entity) {
      entity->SetReplacementComponent(replacementComp);
    }
    return entity;
  };

  m_RoadMap = std::make_shared<FloorMap>(roadFactory, 141.0f, 0.0f, 0.735f,
                                         0.735f, -5.0f); // Render at Z=-5
  m_RoadMap->SetRenderer(&m_Root);
  m_RoadMap->AddToRenderer(); // Add default blocks to root

  for (int i = 0; i < AppUtil::TOTAL_STORY; ++i) {
    auto roadData = AppUtil::MapParser::ParseCSV(
        MAGIC_TOWER_RESOURCE_DIR "/Data/RoadMap" + std::to_string(i) + ".csv");
    if (!roadData.empty()) {
      m_RoadMap->LoadFloorData(roadData, i);
    }
  }
  m_RoadMap->SetAllVisible(false);

  m_ThingsMap = std::make_shared<FloorMap>(
      thingsFactory, 141.0f, 0.0f, 0.735f, 0.735f, -4.0f,
      m_RoadMap->GetBaseBlockSize()); // Use RoadMap spacing
  m_ThingsMap->SetRenderer(&m_Root);
  m_ThingsMap->AddToRenderer(); // Add default blocks to root

  for (int i = 0; i < AppUtil::TOTAL_STORY; ++i) {
    auto thingsData = AppUtil::MapParser::ParseCSV(MAGIC_TOWER_RESOURCE_DIR
                                                   "/Data/ThingsMap" +
                                                   std::to_string(i) + ".csv");
    if (!thingsData.empty()) {
      m_ThingsMap->LoadFloorData(thingsData, i);
    }
  }
  m_ThingsMap->SetAllVisible(false);

  // Test Text
  m_TestText = std::make_shared<NumericDisplayText>(
      MAGIC_TOWER_RESOURCE_DIR "/Font/Cubic_11.ttf", 32);
  m_TestText->SetPrefix("");
  m_TestText->SetNumber(0);
  m_TestText->SetSuffix(" F");
  m_TestText->m_Transform.translation = {150.0f, 335.0f};
  m_TestText->SetZIndex(-3.0f);
  m_TestText->SetVisible(false);
  m_Root.AddChild(m_TestText);

  // Key Displays
  auto initKeyText = [&](std::shared_ptr<NumericDisplayText> &text,
                         const std::string &prefix, const Util::Color &color,
                         float yOffset) {
    text = std::make_shared<NumericDisplayText>(
        MAGIC_TOWER_RESOURCE_DIR "/Font/Cubic_11.ttf", 24);
    text->SetPrefix(prefix);
    text->SetNumber(0);
    text->SetColor(color);
    text->m_Transform.translation = {450.0f, 335.0f - yOffset};
    text->SetZIndex(-3.0f);
    text->SetVisible(false);
    m_Root.AddChild(text);
  };

  initKeyText(m_YellowKeyText, "Yellow: ", Util::Color::FromRGB(255, 255, 0),
              0.0f);
  initKeyText(m_BlueKeyText, "Blue: ", Util::Color::FromRGB(0, 0, 255), 35.0f);
  initKeyText(m_RedKeyText, "Red: ", Util::Color::FromRGB(255, 0, 0), 70.0f);

  // Player Initialization
  m_Player = std::make_shared<Player>();
  m_Player->SyncPosition(m_RoadMap);
  m_Root.AddChild(m_Player);
  m_Player->SetVisible(false);
}

void App::Update() {

  switch (m_GameState) {
  case AppUtil::GameState::MainMenu:
    if (Util::Input::IsKeyDown(Util::Keycode::SPACE)) {
      m_GameState = AppUtil::GameState::Playing;
      m_Background->NextPhase(1);
      m_RoadMap->SetAllVisible(true);
      m_ThingsMap->SetAllVisible(true);
      m_TestText->SetVisible(true);
      m_YellowKeyText->SetVisible(true);
      m_BlueKeyText->SetVisible(true);
      m_RedKeyText->SetVisible(true);
      m_Player->SetVisible(true);
    }
    break;

  case AppUtil::GameState::Playing:
    m_TestText->SetNumber(m_RoadMap->GetCurrentStory());
    if (m_Player) {
      m_YellowKeyText->SetNumber(m_Player->GetYellowKeys());
      m_BlueKeyText->SetNumber(m_Player->GetBlueKeys());
      m_RedKeyText->SetNumber(m_Player->GetRedKeys());
    }

    if (Util::Input::IsKeyDown(Util::Keycode::W) ||
        Util::Input::IsKeyDown(Util::Keycode::UP)) {
      m_Player->Move(0, -1, m_RoadMap, m_ThingsMap);
    }
    if (Util::Input::IsKeyDown(Util::Keycode::S) ||
        Util::Input::IsKeyDown(Util::Keycode::DOWN)) {
      m_Player->Move(0, 1, m_RoadMap, m_ThingsMap);
    }
    if (Util::Input::IsKeyDown(Util::Keycode::A) ||
        Util::Input::IsKeyDown(Util::Keycode::LEFT)) {
      m_Player->Move(-1, 0, m_RoadMap, m_ThingsMap);
    }
    if (Util::Input::IsKeyDown(Util::Keycode::D) ||
        Util::Input::IsKeyDown(Util::Keycode::RIGHT)) {
      m_Player->Move(1, 0, m_RoadMap, m_ThingsMap);
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
               m_RoadMap->GetCurrentStory(), m_Player->GetGridX(),
               m_Player->GetGridY());
    }

    if (Util::Input::IsKeyDown(Util::Keycode::NUM_8) ||
        Util::Input::IsKeyDown(Util::Keycode::KP_8)) {
      ChangeFloor(1);
    }
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_2) ||
        Util::Input::IsKeyDown(Util::Keycode::KP_2)) {
      ChangeFloor(-1);
    }
    break;
  }

  m_RoadMap->Update();
  m_ThingsMap->Update();

  m_TestText->UpdateDisplayText();
  m_YellowKeyText->UpdateDisplayText();
  m_BlueKeyText->UpdateDisplayText();
  m_RedKeyText->UpdateDisplayText();
  m_Root.Update();

  /*
   * Do not touch the code below as they serve the purpose for
   * closing the window.
   */
  if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
    m_CurrentState = State::END;
  }
}

void App::End() { // NOLINT(this method will mutate members in the future)
  LOG_TRACE("End");
}

void App::ChangeFloor(int delta) {
  int nextStory = m_RoadMap->GetCurrentStory() + delta;
  if (nextStory >= 0 && nextStory < AppUtil::TOTAL_STORY) {
    m_RoadMap->SwitchStory(nextStory);
    m_ThingsMap->SwitchStory(nextStory);
    LOG_INFO("Switched to story {}", nextStory);
    if (m_Player) {
      m_Player->SyncPosition(m_RoadMap);
    }
    LOG_INFO("Player Position (Floor Switch): Floor {}, Grid({}, {})",
             m_RoadMap->GetCurrentStory(), m_Player->GetGridX(),
             m_Player->GetGridY());
  }
}
