#include "App.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

void App::Start() {
  LOG_TRACE("Start");
  m_CurrentState = State::UPDATE;

  m_Background = std::make_shared<Background>();
  m_Root.AddChild(m_Background);

  // Initialize FloorMap and parse data
  m_FloorMap = std::make_shared<FloorMap>(141.0f, 0.0f, 0.735f, 0.735f);
  auto mapData = AppUtil::MapParser::ParseCSV(MAGIC_TOWER_RESOURCE_DIR
                                              "/Data/RoadMap0.csv");
  m_FloorMap->LoadFloorData(mapData);
  m_FloorMap->SetAllBlocksVisible(false);

  m_Root.AddChild(m_FloorMap);
}

void App::Update() {

  switch (m_GameState) {
  case AppUtil::GameState::MainMenu:
    if (Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
      m_GameState = AppUtil::GameState::Playing;
      m_Background->NextPhase(1);
      m_FloorMap->SetAllBlocksVisible(true);
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
