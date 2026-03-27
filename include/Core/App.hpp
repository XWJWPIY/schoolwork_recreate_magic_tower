#ifndef APP_HPP
#define APP_HPP

#include "Core/AppUtil.hpp"
#include "UI/Background.hpp"
#include "Core/FloorMap.hpp"
#include "UI/NumericDisplayText.hpp"
#include "UI/ItemNoticeUI.hpp"
#include "Objects/Player.hpp"
#include "UI/StatusUI.hpp"
#include "Objects/Shop.hpp"
#include "UI/UIComponent.hpp"
#include "UI/FlyUI.hpp"
#include "UI/NoticeUI.hpp"
#include "UI/DialogueUI.hpp"
#include "Core/EntityFactory.hpp"
#include "pch.hpp" // IWYU pragma: export
#include <memory>
#include <vector>

class DialogueUI;

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
  void SetFloor(int story);
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
  std::shared_ptr<Background>  m_background;
  std::shared_ptr<FloorMap>    m_road_map;
  std::shared_ptr<FloorMap>    m_things_map;
  std::shared_ptr<StatusUI>    m_status_ui;
  std::shared_ptr<Player>      m_player;
  std::shared_ptr<FlyUI>       m_fly_ui;
  std::shared_ptr<NoticeUI>    m_notice_ui;
  std::shared_ptr<ItemNoticeUI> m_item_notice_ui;
  std::shared_ptr<DialogueUI> m_dialogue_ui;

  float m_item_notice_timer = 0.0f;
  float m_loading_timer  = 0.0f;
  int   m_loading_frame  = 0;

  // UI Components list for unified management
  std::vector<std::shared_ptr<UIComponent>> m_ui_components;

  // Active shop session (non-owning, entity is owned by FloorMap)
  Shop* m_active_shop = nullptr;

  std::unique_ptr<EntityFactory> m_entity_factory;
};

#endif
