#include "DynamicReplacementComponent.hpp"

DynamicReplacementComponent::DynamicReplacementComponent(ReplacementCallback callback)
    : m_Callback(callback) {}

void DynamicReplacementComponent::ReplaceWith(int x, int y, int id) const {
    if (m_Callback) {
        m_Callback(x, y, id);
    }
}
