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
  void TeleportToFloor(int targetStory, int targetStairId);
  void ShowItemNotice(const std::string& text);
  void HideItemNotice();
  void Restart();

private:
  void ValidTask();
  void InitializeGame();

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
  
  float m_item_notice_timer = 0.0f;
  float m_loading_timer = 0.0f;
  int m_loading_frame = 0;

  int m_shop_selection = 0;
  int m_current_shop_id = -1;
  void ApplyShopScaling(AppUtil::ShopData& data, int id, int times);
  AppUtil::ShopData m_current_shop_data;
};

#endif
