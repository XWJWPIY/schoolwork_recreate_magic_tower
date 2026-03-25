#include "AllObjects.hpp"
#include "AppUtil.hpp"
#include "Util/Image.hpp"

AllObjects::AllObjects(int initialId) : m_object_id(initialId) {}

AllObjects::AllObjects(const std::shared_ptr<Core::Drawable> &drawable,
                       const float zIndex, int initialId)
    : Util::GameObject(drawable, zIndex), m_object_id(initialId) {}

void AllObjects::SetObjectId(int newId) {
    m_object_id = newId;

    // Default for empty/invisible (ID 0)
    if (newId == 0) {
        SetVisible(false);
        m_is_passable = true;
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

void AllObjects::ObjectUpdate() {
    // Shared Central Sync logic:
    // If we have an animation and it's in PAUSE state, it means it's a scene tile
    // that should be synced with the global clock.
    if (m_animation && m_animation->GetState() == Util::Animation::State::PAUSE) {
        int frame = AppUtil::TileAnimationManager::GetGlobalFrame2(500);
        m_animation->SetCurrentFrame(frame - 1);
    }
}

void AllObjects::SetupAnimation(int id, bool looping, int intervalMs) {
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
