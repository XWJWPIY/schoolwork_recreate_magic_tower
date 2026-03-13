#ifndef SHOP_HPP
#define SHOP_HPP

#include "Entity.hpp"
#include "Util/Logger.hpp"
#include "pch.hpp"

#include <string>

class Shop : public Entity {
public:
  Shop(int id);
  ~Shop() override = default;

  void reaction() override;
};

#endif // SHOP_HPP
