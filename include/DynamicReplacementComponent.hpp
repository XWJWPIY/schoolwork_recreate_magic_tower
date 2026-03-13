#ifndef DYNAMIC_REPLACEMENT_COMPONENT_HPP
#define DYNAMIC_REPLACEMENT_COMPONENT_HPP

#include <functional>

/**
 * @brief Logic component to handle dynamic replacement of an object on the FloorMap.
 */
class DynamicReplacementComponent {
public:
    using ReplacementCallback = std::function<void(int x, int y, int id)>;

    DynamicReplacementComponent(ReplacementCallback callback);

    void ReplaceWith(int x, int y, int id) const;

private:
    ReplacementCallback m_Callback;
};

#endif // DYNAMIC_REPLACEMENT_COMPONENT_HPP
