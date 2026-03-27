#include "Core/EntityFactory.hpp"
#include "Core/AppUtil.hpp"
#include "Objects/Door.hpp"
#include "Objects/Enemy.hpp"
#include "Objects/Item.hpp"
#include "Objects/MapBlock.hpp"
#include "Objects/NPC.hpp"
#include "Objects/Shop.hpp"
#include "Objects/Stair.hpp"
#include "Objects/Trigger.hpp"
#include "Systems/DynamicReplacementComponent.hpp"

EntityFactory::EntityFactory(const Callbacks& callbacks)
    : m_callbacks(callbacks) {}

std::shared_ptr<Entity> EntityFactory::CreateRoadBlock(int id) {
    return std::make_shared<MapBlock>(id);
}

std::shared_ptr<Entity> EntityFactory::CreateEntity(int id) {
    std::shared_ptr<Entity> entity;

    // Factory logic grouped by ID ranges
    if (id >= 200 && id < 300) {
        // Items
        entity = std::make_shared<Item>(id, m_callbacks.showItemNotice);

    } else if (id >= 300 && id < 400) {
        // Doors
        entity = std::make_shared<Door>(id);

    } else if (id >= 400 && id < 500) {
        // Enemies
        entity = std::make_shared<Enemy>(id);

    } else if (id >= 500 && id < 600) {
        // NPCs
        entity = std::make_shared<NPC>(
            id, [this](std::shared_ptr<NPC> npc, const std::string& path) {
                if (m_callbacks.startScript) {
                    std::string scriptName = std::to_string(m_callbacks.getCurrentStory()) + "_" + path;
                    m_callbacks.startScript(npc, scriptName);
                }
            });

    } else if (id >= 600 && id < 700) {
        // Shops
        entity = std::make_shared<Shop>(
            id,
            [this](Shop& s) { if (m_callbacks.openShop) m_callbacks.openShop(s); },
            [this]() { if (m_callbacks.closeShop) m_callbacks.closeShop(); });

    } else if (id >= 700 && id < 800) {
        // Stairs
        entity = std::make_shared<Stair>(
            id, [this](int val, bool rel) {
                if (rel) {
                    if (m_callbacks.changeFloor) m_callbacks.changeFloor(val);
                } else {
                    if (m_callbacks.setFloor) m_callbacks.setFloor(val);
                }
            });

    } else if (id >= 800 && id < 900) {
        // Triggers
        entity = std::make_shared<Trigger>(
            id, [this](std::shared_ptr<Trigger> t, const std::string& path) {
                if (m_callbacks.startScript) {
                    std::string scriptName = std::to_string(m_callbacks.getCurrentStory()) + "_" + path;
                    m_callbacks.startScript(t, scriptName);
                }
            });

    } else {
        // Fallback for ID 0 or unknown
        entity = std::make_shared<Entity>(id, AppUtil::GetStaticResourcePath("bmp/Road/road1.bmp"), true);
        if (id == 0) entity->SetVisible(false);
    }

    if (entity && m_replacement_comp) {
        entity->SetReplacementComponent(m_replacement_comp);
    }

    return entity;
}
