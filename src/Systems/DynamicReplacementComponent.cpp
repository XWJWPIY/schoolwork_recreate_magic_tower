#include "Systems/DynamicReplacementComponent.hpp"

DynamicReplacementComponent::DynamicReplacementComponent(ReplacementCallback callback)
    : m_callback(callback) {}

void DynamicReplacementComponent::ReplaceWith(int x, int y, int id) const {
    if (m_callback) {
        m_callback(x, y, id);
    }
}
