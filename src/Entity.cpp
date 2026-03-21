#include "Entity.hpp"
#include "AppUtil.hpp"
#include "Util/Logger.hpp"
#include "Util/Image.hpp"

Entity::Entity(int initialId, const std::string &imagePath, bool canReact) 
    : AllObjects(initialId) {
    SetZIndex(-4);
    UpdateProperties(initialId);
    m_can_react = canReact;
}

void Entity::SetObjectId(int newId) {
    AllObjects::SetObjectId(newId);
    UpdateProperties(newId);
}

void Entity::UpdateProperties(int id) {
    auto it = AppUtil::GlobalObjectRegistry.find(id);
    if (it != AppUtil::GlobalObjectRegistry.end()) {
        const auto& meta = it->second;
        m_is_passable = meta.is_passable;
        SetVisible(true);

        if (meta.is_animated) {
            m_current_frame = AppUtil::TileAnimationManager::GetGlobalFrame2(500);
            std::string base = MAGIC_TOWER_RESOURCE_DIR + AppUtil::GetIdResourcePath(id);
            std::string prefix = base.substr(0, base.length() - 5); // remove 1.bmp
            SetDrawable(std::make_shared<Util::Image>(prefix + std::to_string(m_current_frame) + ".bmp"));
        } else {
            SetDrawable(std::make_shared<Util::Image>(MAGIC_TOWER_RESOURCE_DIR + AppUtil::GetIdResourcePath(id)));
        }
    } else if (id == 0) {
        SetVisible(false);
        m_is_passable = true;
    }
}

void Entity::ObjectUpdate() {
    auto it = AppUtil::GlobalObjectRegistry.find(m_object_id);
    if (it != AppUtil::GlobalObjectRegistry.end() && it->second.is_animated) {
        int global_frame = AppUtil::TileAnimationManager::GetGlobalFrame2(500);
        if (m_current_frame != global_frame) {
            m_current_frame = global_frame;
            auto image = std::dynamic_pointer_cast<Util::Image>(m_Drawable);
            if (image) {
                std::string base = MAGIC_TOWER_RESOURCE_DIR + AppUtil::GetIdResourcePath(m_object_id);
                std::string prefix = base.substr(0, base.length() - 5);
                image->SetImage(prefix + std::to_string(m_current_frame) + ".bmp");
            }
        }
    }
}

void Entity::TriggerReplacement(int targetId) {
    if (m_replacement_comp) {
        m_replacement_comp->ReplaceWith(m_grid_x, m_grid_y, targetId);
    }
}
