#include "App.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

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

  // Factory for ThingsMap (for now also creates MapBlocks as placeholders,
  // until actual Items/Monsters/Player are fully integrated to factory)
  FloorMap::BlockFactory thingsFactory =
      [](int id) -> std::shared_ptr<AllObjects> {
    if (id == 0)
      return nullptr; // 0 means empty space in ThingsMap
    return std::make_shared<MapBlock>(
        id); // Temporarily using MapBlock to render something
  };

  m_RoadMap =
      std::make_shared<FloorMap>(roadFactory, 141.0f, 0.0f, 0.735f, 0.735f);
  m_RoadMap->SetRenderer(&m_Root);
  m_RoadMap->AddToRenderer(); // Add default blocks to root

  auto roadData = AppUtil::MapParser::ParseCSV(MAGIC_TOWER_RESOURCE_DIR
                                               "/Data/RoadMap0.csv");
  m_RoadMap->LoadFloorData(roadData);
  m_RoadMap->SetAllBlocksVisible(false);

  // m_ThingsMap =
  //     std::make_shared<FloorMap>(thingsFactory, 141.0f, 0.0f, 0.735f,
  //     0.735f);
  // m_ThingsMap->SetRenderer(&m_Root);
  // m_ThingsMap->AddToRenderer(); // Add default blocks to root

  // auto thingsData = AppUtil::MapParser::ParseCSV(MAGIC_TOWER_RESOURCE_DIR
  //                                                "/Data/ThingsMap0.csv");
  // m_ThingsMap->LoadFloorData(thingsData);
  // m_ThingsMap->SetAllBlocksVisible(false);
}

void App::Update() {

  switch (m_GameState) {
  case AppUtil::GameState::MainMenu:
    if (Util::Input::IsKeyDown(Util::Keycode::SPACE)) {
      m_GameState = AppUtil::GameState::Playing;
      m_Background->NextPhase(1);
      m_RoadMap->SetAllBlocksVisible(true);
      // m_ThingsMap->SetAllBlocksVisible(true);
    }
    break;

  case AppUtil::GameState::Playing:
    break;
  }

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
