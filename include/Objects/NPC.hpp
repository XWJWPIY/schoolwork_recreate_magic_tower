#ifndef NPC_HPP
#define NPC_HPP

#include "Objects/Entity.hpp"
#include "Util/Logger.hpp"
#include "pch.hpp"

#include <string>

#include <functional>

class NPC : public Entity {
public:
  using TalkCallback = std::function<void(std::shared_ptr<NPC>, const std::string&)>;

  NPC(int id, TalkCallback callback);
  ~NPC() override = default;

  void Reaction(std::shared_ptr<Player> player) override;

private:
  TalkCallback m_talk_callback;
};

#endif // NPC_HPP
