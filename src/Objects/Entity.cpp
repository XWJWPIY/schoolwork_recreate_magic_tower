#include "Objects/Entity.hpp"
#include "Core/AppUtil.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"

/* --- 動畫與同步機制註記 ---
 * 1. NPC、Shop：目前的影像資源具備多幀循環（frames > 1），統一由 Entity 基底類別
 *    透過 GLOBAL_SYNC 模式與「地磚/場景物件」進行全域同步控制。
 * 2. Enemy：未來實作呼吸動畫時，同樣需在 CSV 標註 Animation > 1，
 *    系統將自動納入全域同步軌道。
 * 3. Item、Stair：由於 CSV 標註為 1 幀，系統將保持靜態顯示（STATIC）。
 */

Entity::Entity(int initialId) {
    SetObjectId(initialId);
}

Entity::Entity(int initialId, bool canReact) 
    : m_object_id(initialId), m_can_react(canReact) {
    SetZIndex(-4);
    SetObjectId(initialId); // 觸發資源載入與動畫初始化
}

Entity::Entity(const std::shared_ptr<Core::Drawable> &drawable,
               const float zIndex, int initialId)
    : Util::GameObject(drawable, zIndex) {
    SetObjectId(initialId);
}

void Entity::SetObjectId(int newId) {
    m_object_id = newId;

    // Default for empty/invisible (ID 0)
    if (newId == 0) {
        SetVisible(false);
        m_is_passable = true;
        // Optional: set a default image for debugging
        SetDrawable(std::make_shared<Util::Image>(AppUtil::GetStaticResourcePath("bmp/Road/road1.bmp")));
        return;
    }

    auto it = AppUtil::GlobalObjectRegistry.find(newId);
    if (it != AppUtil::GlobalObjectRegistry.end()) {
        const auto& meta = it->second;
        m_is_passable = meta.is_passable;
        SetVisible(true);

        if (meta.frames > 1) {
            SetupAnimation(newId, true, 500); // looping=true, centrally controlled
            m_animation->Pause();              // Don't let it run on its own
            int frame = AppUtil::TileAnimationManager::GetGlobalFrame2(500);
            m_animation->SetCurrentFrame(frame - 1);
        } else {
            SetDrawable(std::make_shared<Util::Image>(AppUtil::GetFullResourcePath(newId)));
        }
    }
}

void Entity::ObjectUpdate() {
    // Shared Central Sync logic:
    // If we have an animation and it's in PAUSE state, it means it's a scene tile
    // that should be synced with the global clock.
    if (m_animation && m_animation->GetState() == Util::Animation::State::PAUSE) {
        int frame = AppUtil::TileAnimationManager::GetGlobalFrame2(500);
        m_animation->SetCurrentFrame(frame - 1);
    }
}

void Entity::SetupAnimation(int id, bool looping, int intervalMs) {
    auto it = AppUtil::GlobalObjectRegistry.find(id);
    if (it == AppUtil::GlobalObjectRegistry.end()) return;

    const auto& meta = it->second;
    m_base_image_path = AppUtil::GetBaseImagePath(id);

    std::vector<std::string> paths;
    for (int i = 1; i <= meta.frames; i++) {
        paths.push_back(AppUtil::GetPhaseImagePath(m_base_image_path, i));
    }

    bool autoPlay = looping; // Scene tiles auto-play; one-shot waits for Play()
    m_animation = std::make_shared<Util::Animation>(paths, autoPlay, intervalMs, looping);
    SetDrawable(m_animation);
}

void Entity::Reaction(std::shared_ptr<Player> player) {
    // Default implementation does nothing
}

void Entity::TriggerReplacement(int targetId) {
    if (m_replacement_comp) {
        m_replacement_comp->ReplaceWith(m_grid_x, m_grid_y, targetId);
    }
}

void Entity::ForEachAttribute(const std::function<void(AppUtil::Effect, int)>& callback) const {
    auto it = AppUtil::GlobalObjectRegistry.find(m_object_id);
    if (it == AppUtil::GlobalObjectRegistry.end() || !callback) return;

    const auto& meta = it->second;
    for (const auto& [attrId, valStr] : meta.attributes) {
        if (valStr.empty()) continue;
        AppUtil::Effect eff = AppUtil::AttributeRegistry::ToEffect(attrId);
        if (eff != AppUtil::Effect::NONE) {
            try {
                int val = std::stoi(valStr);
                if (val != 0) callback(eff, val);
            } catch (...) {
                // Ignore malformed attribute values
            }
        }
    }
}
