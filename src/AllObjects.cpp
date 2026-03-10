#include "AllObjects.hpp"

AllObjects::AllObjects(int initialId) : m_ObjectId(initialId) {}

AllObjects::AllObjects(const std::shared_ptr<Core::Drawable> &drawable,
                       const float zIndex, int initialId)
    : Util::GameObject(drawable, zIndex), m_ObjectId(initialId) {}
