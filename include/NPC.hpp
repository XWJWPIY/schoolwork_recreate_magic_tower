#ifndef NPC_HPP
#define NPC_HPP

#include "Entity.hpp"
#include "Util/Logger.hpp"
#include "pch.hpp"

#include <string>

#include <functional>

class NPC : public Entity {
public:
  typedef std::function<void(std::shared_ptr<NPC>, const std::string&)> TalkCallback;

  NPC(int id, TalkCallback callback);
  ~NPC() override = default;

  void Reaction(std::shared_ptr<Player> player) override;

private:
  TalkCallback m_talk_callback;
};

#endif // NPC_HPP
