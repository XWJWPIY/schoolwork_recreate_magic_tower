#ifndef ITEM_HPP
#define ITEM_HPP

#include "Entity.hpp"
#include "Util/Logger.hpp"
#include "pch.hpp"

#include <string>
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
