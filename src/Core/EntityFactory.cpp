#include "Core/EntityFactory.hpp"
#include "Core/AppUtil.hpp"
#include "Objects/Door.hpp"
#include "Objects/Enemy.hpp"
#include "Objects/Item.hpp"
#include "Objects/MapBlock.hpp"
#include "Objects/NPC.hpp"
#include "Objects/Shop.hpp"
#include "Objects/Stair.hpp"

#include "Systems/DynamicReplacementComponent.hpp"

EntityFactory::EntityFactory(const Callbacks& callbacks)
    : m_callbacks(callbacks) {}

std::shared_ptr<Entity> EntityFactory::CreateRoadBlock(int id) {
    return std::make_shared<MapBlock>(id);
}

std::shared_ptr<Entity> EntityFactory::CreateEntity(int id) {
    std::shared_ptr<Entity> entity;

    // Dispatch to specialized creators based on ID ranges
    if (id >= 1 && id < 200)         entity = CreateRoadBlock(id);
    else if (id >= 200 && id < 300) entity = CreateItem(id);
    else if (id >= 300 && id < 400) entity = CreateDoor(id);
    else if (id >= 400 && id < 500) entity = CreateEnemy(id);
    else if (id >= 500 && id < 600) entity = CreateNPC(id);
    else if (id >= 600 && id < 700) entity = CreateShop(id);
    else if (id >= 700 && id < 800) entity = CreateStair(id);
    else if (id >= 800 && id < 900) entity = CreateNPC(id);
    else {
        // Fallback for ID 0 or unknown: create a MapBlock for consistency if it's in the road range,
        // otherwise just a basic Entity.
        if (id == 0) {
            entity = CreateRoadBlock(0);
            entity->SetVisible(false);
        } else {
            entity = std::make_shared<Entity>(id, true);
        }
    }

    if (entity && m_replacement_comp) {
        entity->SetReplacementComponent(m_replacement_comp);
    }

    return entity;
}

std::shared_ptr<Entity> EntityFactory::CreateItem(int id) {
    return std::make_shared<Item>(id, m_callbacks.showItemNotice);
}

std::shared_ptr<Entity> EntityFactory::CreateDoor(int id) {
    return std::make_shared<Door>(id);
}

std::shared_ptr<Entity> EntityFactory::CreateEnemy(int id) {
    auto it = AppUtil::GlobalObjectRegistry.find(id);
    if (it != AppUtil::GlobalObjectRegistry.end()) {
        if (it->second.GetInt("Core_ID", 0) > 0) {
            return std::make_shared<EnemyPart>(id);
        }
    }
    auto enemy = std::make_shared<Enemy>(id);
    enemy->SetBattleCallback(m_callbacks.startBattle);
    return enemy;
}

std::shared_ptr<Entity> EntityFactory::CreateNPC(int id) {
    return std::make_shared<NPC>(
        id, [this](std::shared_ptr<NPC> npc, const std::string& path) {
            if (m_callbacks.startScript) {
                std::string scriptName = std::to_string(m_callbacks.getCurrentStory()) + "_" + path;
                m_callbacks.startScript(npc, scriptName);
            }
        });
}

std::shared_ptr<Entity> EntityFactory::CreateShop(int id) {
    return std::make_shared<Shop>(
        id,
        [this](Shop& s) { if (m_callbacks.openShop) m_callbacks.openShop(s); },
        [this]() { if (m_callbacks.closeShop) m_callbacks.closeShop(); });
}

std::shared_ptr<Entity> EntityFactory::CreateStair(int id) {
    return std::make_shared<Stair>(
        id, [this](int val, bool rel) {
            if (rel) {
                if (m_callbacks.changeFloor) m_callbacks.changeFloor(val);
            } else {
                if (m_callbacks.setFloor) m_callbacks.setFloor(val);
            }
        });
}


