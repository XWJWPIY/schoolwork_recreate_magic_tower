#include "App.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

#include "Entity.hpp"
#include "MapBlock.hpp"

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

  // Factory for ThingsMap (Always create Entity, even for ID 0)
  FloorMap::BlockFactory thingsFactory =
      [](int id) -> std::shared_ptr<AllObjects> {
    return std::make_shared<Entity>(id);
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
    }
    break;

  case AppUtil::GameState::Playing:
    m_TestText->SetNumber(m_RoadMap->GetCurrentStory());
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_8) ||
        Util::Input::IsKeyDown(Util::Keycode::KP_8)) {
      int nextStory = m_RoadMap->GetCurrentStory() + 1;
      if (nextStory < AppUtil::TOTAL_STORY) {
        m_RoadMap->SwitchStory(nextStory);
        m_ThingsMap->SwitchStory(nextStory);
        LOG_INFO("Switched to story {}", nextStory);
      }
    }
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_2) ||
        Util::Input::IsKeyDown(Util::Keycode::KP_2)) {
      int prevStory = m_RoadMap->GetCurrentStory() - 1;
      if (prevStory >= 0) {
        m_RoadMap->SwitchStory(prevStory);
        m_ThingsMap->SwitchStory(prevStory);
        LOG_INFO("Switched to story {}", prevStory);
      }
    }
    break;
  }

  m_TestText->UpdateDisplayText();
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
