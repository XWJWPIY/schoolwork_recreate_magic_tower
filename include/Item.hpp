#ifndef ITEM_HPP
#define ITEM_HPP

#include "Entity.hpp"
#include "Util/Logger.hpp"
#include "pch.hpp"

#include <string>

class Item : public Entity {
public:
  Item(int id);
  ~Item() override = default;

  void reaction() override;
};

#endif // ITEM_HPP
