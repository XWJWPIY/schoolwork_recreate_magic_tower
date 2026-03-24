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

        if (meta.frames > 1) {
            m_current_frame = AppUtil::TileAnimationManager::GetGlobalFrame2(500);
            std::string basePath = AppUtil::GetBaseImagePath(id);
            SetDrawable(std::make_shared<Util::Image>(AppUtil::GetPhaseImagePath(basePath, m_current_frame)));
        } else {
            SetDrawable(std::make_shared<Util::Image>(AppUtil::GetFullResourcePath(id)));
        }
    } else if (id == 0) {
        SetVisible(false);
        m_is_passable = true;
        // Use road1.bmp as size template for empty entity slot
        SetDrawable(std::make_shared<Util::Image>(AppUtil::GetStaticResourcePath("bmp/Road/road1.bmp")));
    }
}

void Entity::ObjectUpdate() {
    auto it = AppUtil::GlobalObjectRegistry.find(m_object_id);
    if (it != AppUtil::GlobalObjectRegistry.end() && it->second.frames > 1) {
        int global_frame = AppUtil::TileAnimationManager::GetGlobalFrame2(500);
        if (m_current_frame != global_frame) {
            m_current_frame = global_frame;
            auto image = std::dynamic_pointer_cast<Util::Image>(m_Drawable);
            if (image) {
                std::string base = AppUtil::GetFullResourcePath(m_object_id);
                std::string prefix = base.substr(0, base.length() - 5);
                image->SetImage(prefix + std::to_string(m_current_frame) + ".bmp");
            }
        }
    }
}

void Entity::Reaction(std::shared_ptr<Player> player) {
    // Default implementation does nothing
}

void Entity::TriggerReplacement(int targetId) {
    if (m_replacement_comp) {
        m_replacement_comp->ReplaceWith(m_grid_x, m_grid_y, targetId);
    }
}
