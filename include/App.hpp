#ifndef APP_HPP
#define APP_HPP

#include "AppUtil.hpp"
#include "Background.hpp"
#include "FloorMap.hpp"
#include "NumericDisplayText.hpp"
#include "Util/Renderer.hpp"
#include "Util/Text.hpp"
#include "pch.hpp" // IWYU pragma: export
#include <memory>

class App {

public:
  enum class State {
    START,
    UPDATE,
    END,
  };

  State GetCurrentState() const { return m_CurrentState; }

  void Start();

  void Update();

  void End(); // NOLINT(readability-convert-member-functions-to-static)

private:
  void ValidTask();

private:
  State m_CurrentState = State::START;
  AppUtil::GameState m_GameState = AppUtil::GameState::MainMenu;

  Util::Renderer m_Root;
  std::shared_ptr<Background> m_Background;
  std::shared_ptr<FloorMap> m_RoadMap;
  std::shared_ptr<FloorMap> m_ThingsMap;
  std::shared_ptr<NumericDisplayText> m_TestText;
};

#endif
