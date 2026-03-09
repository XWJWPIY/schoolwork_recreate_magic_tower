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
}

void App::Update() {

  switch (m_GameState) {
  case AppUtil::GameState::MainMenu:
    // TODO: 實作主頁邏輯 (例如：按 Enter 開始遊戲)
    if (Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
      m_GameState = AppUtil::GameState::Playing;
      // 按下 Enter 後，切換到第二張背景 (phase 1)
      m_Background->NextPhase(1);
    }
    break;

  case AppUtil::GameState::Playing:
    // TODO: 實作遊戲進行中的邏輯 (角色移動、戰鬥等)
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
