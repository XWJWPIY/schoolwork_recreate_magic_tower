#ifndef APP_HPP
#define APP_HPP

#include "AppUtil.hpp"
#include "Background.hpp"
#include "FloorMap.hpp"
#include "NumericDisplayText.hpp"
#include "MenuUI.hpp"
#include "Player.hpp"
#include "StatusUI.hpp"
#include "Util/Renderer.hpp"
#include "Util/Text.hpp"
#include "pch.hpp" // IWYU pragma: export
#include <memory>

class App {

public:
  enum class STATE {
    START,
    UPDATE,
    END,
  };

  STATE GetCurrentState() const { return m_current_state; }

  void Start();

  void Update();

  void End(); // NOLINT(readability-convert-member-functions-to-static)

  void ChangeFloor(int delta);

private:
  void ValidTask();

private:
  STATE m_current_state = STATE::START;
  AppUtil::GameState m_game_state = AppUtil::GameState::MAIN_MENU;

  Util::Renderer m_root;
  std::shared_ptr<Background> m_background;
  std::shared_ptr<FloorMap> m_road_map;
  std::shared_ptr<FloorMap> m_things_map;
  std::shared_ptr<StatusUI> m_status_ui;
  std::shared_ptr<Player> m_player;
  std::shared_ptr<MenuUI> m_menu_ui;
  int m_preview_floor = 0;
};

#endif
