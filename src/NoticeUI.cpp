#include "NoticeUI.hpp"
#include "AppUtil.hpp"

NoticeUI::NoticeUI() {
    m_image = std::make_shared<Util::Image>(MAGIC_TOWER_RESOURCE_DIR "/bmp/Special/notice.bmp");
    SetDrawable(m_image);
    
    // Position in center of screen
    m_Transform.translation = {141.0f, 0.0f};
    m_Transform.scale = {0.735f, 0.735f};
    
    // Highest Z-index to be on top of everything
    SetZIndex(-1.0f);
    
    SetVisible(false);
}

void NoticeUI::SetVisible(bool visible) {
    m_Visible = visible;
}
