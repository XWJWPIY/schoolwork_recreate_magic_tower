#ifndef UI_COMPONENT_HPP
#define UI_COMPONENT_HPP

#include "Util/Renderer.hpp"

/**
 * @brief 所有 UI 元件的抽象基底類別
 */
class UIComponent {
public:
    virtual ~UIComponent() = default;

    /**
     * @brief 每幀執行的核心邏輯 (包含 HandleInput 與 Update)
     */
    virtual void run() = 0;

    /**
     * @brief 設定 UI 是否可見
     */
    virtual void SetVisible(bool visible) = 0;

    /**
     * @brief 將 UI 元件加入 PTSD 的渲染器
     */
    virtual void AddToRoot(Util::Renderer& root) = 0;

    /**
     * @brief 回傳目前 UI 是否處於活動狀態
     */
    virtual bool IsActive() const { return m_visible; }

    /**
     * @brief 是否攔截輸入並暫停地圖邏輯 (1=會 0=不會)
     */
    virtual bool IsIntercepting() const { return false; }

protected:
    bool m_visible = false;
};

#endif // UI_COMPONENT_HPP
