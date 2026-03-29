#ifndef ENTITY_FACTORY_HPP
#define ENTITY_FACTORY_HPP

#include "Objects/Entity.hpp"
#include <functional>
#include <memory>
#include <string>

// Forward declarations
class Shop;

/**
 * @brief Factory class to centralize and decouple the creation of game entities.
 * Handles specialized entity construction and dependency injection via callbacks.
 */
class EntityFactory {
public:
    struct Callbacks {
        std::function<void(const std::string&)> showItemNotice;
        std::function<void(std::shared_ptr<Entity>, const std::string&)> startScript;
        std::function<int()> getCurrentStory;
        std::function<void(int)> changeFloor;
        std::function<void(int)> setFloor;
        std::function<void(Shop&)> openShop;
        std::function<void()> closeShop;
    };

    explicit EntityFactory(const Callbacks& callbacks);

    /**
     * @brief Create a specialized entity based on its ID (ThingsMap or RoadMap).
     */
    std::shared_ptr<Entity> CreateEntity(int id);

    /**
     * @brief Set the dynamic replacement component for coordinate-based replacements.
     */
    void SetReplacementComponent(std::shared_ptr<class DynamicReplacementComponent> comp) {
        m_replacement_comp = comp;
    }

private:
    std::shared_ptr<Entity> CreateRoadBlock(int id);
    std::shared_ptr<Entity> CreateItem(int id);
    std::shared_ptr<Entity> CreateDoor(int id);
    std::shared_ptr<Entity> CreateEnemy(int id);
    std::shared_ptr<Entity> CreateNPC(int id);
    std::shared_ptr<Entity> CreateShop(int id);
    std::shared_ptr<Entity> CreateStair(int id);


    Callbacks m_callbacks;
    std::shared_ptr<class DynamicReplacementComponent> m_replacement_comp;
};

#endif // ENTITY_FACTORY_HPP
