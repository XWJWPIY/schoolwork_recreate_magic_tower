#ifndef ITEM_HPP
#define ITEM_HPP

#include "Objects/Entity.hpp"
#include "Util/Logger.hpp"
#include "Core/AppUtil.hpp"
#include <functional>

class Item : public Entity {
public:
  using NoticeCallback = std::function<void(const std::string&)>;

  Item(int id, NoticeCallback callback = nullptr);
  ~Item() override = default;

  void Reaction(std::shared_ptr<Player> player) override;

private:
  NoticeCallback m_notice_callback;
};

#endif // ITEM_HPP
