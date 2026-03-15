#ifndef STATUS_UI_HPP
#define STATUS_UI_HPP

#include "NumericDisplayText.hpp"
#include "Player.hpp"
#include "Util/Renderer.hpp"

class StatusUI {
public:
    StatusUI(unsigned int fontSize = 36);
    ~StatusUI() = default;

    void Update(const std::shared_ptr<Player>& player, int floorNum);
    void SetVisible(bool visible);
    
    // Getter for the root group if needed, or we can just pass the root to it
    void AddToRoot(Util::Renderer& root);

private:
    std::shared_ptr<NumericDisplayText> m_YellowKeyText;
    std::shared_ptr<NumericDisplayText> m_BlueKeyText;
    std::shared_ptr<NumericDisplayText> m_RedKeyText;
    std::shared_ptr<NumericDisplayText> m_FloorText;
    
    unsigned int m_DefaultFontSize;

    // Helper to initialize text objects
    void InitNumericText(std::shared_ptr<NumericDisplayText>& text, 
                         float x, float y, const Util::Color& color, unsigned int size);
};

#endif // STATUS_UI_HPP
