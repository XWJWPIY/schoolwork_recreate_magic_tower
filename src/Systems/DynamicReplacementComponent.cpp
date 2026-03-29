#include "Systems/DynamicReplacementComponent.hpp"

DynamicReplacementComponent::DynamicReplacementComponent(ReplacementCallback callback, FindEntityCallback find_callback)
    : m_callback(callback), m_find_callback(find_callback) {}

void DynamicReplacementComponent::ReplaceWith(int x, int y, int id) const {
    if (m_callback) {
        m_callback(x, y, id);
    }
}

std::shared_ptr<Entity> DynamicReplacementComponent::FindEntityById(int id) const {
    if (m_find_callback) {
        return m_find_callback(id);
    }
    return nullptr;
}
