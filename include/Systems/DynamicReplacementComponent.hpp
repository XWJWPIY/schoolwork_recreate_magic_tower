#ifndef DYNAMIC_REPLACEMENT_COMPONENT_HPP
#define DYNAMIC_REPLACEMENT_COMPONENT_HPP

#include <functional>

#include <memory>

class Entity;

/**
 * @brief Logic component to handle dynamic replacement of an object on the FloorMap.
 */
class DynamicReplacementComponent {
public:
    using ReplacementCallback = std::function<void(int x, int y, int id)>;
    using FindEntityCallback = std::function<std::shared_ptr<Entity>(int id)>;

    DynamicReplacementComponent(ReplacementCallback callback, FindEntityCallback find_callback = nullptr);

    void ReplaceWith(int x, int y, int id) const;
    std::shared_ptr<Entity> FindEntityById(int id) const;

private:
    ReplacementCallback m_callback;
    FindEntityCallback m_find_callback;
};

#endif // DYNAMIC_REPLACEMENT_COMPONENT_HPP
