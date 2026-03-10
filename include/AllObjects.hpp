#ifndef ALL_OBJECTS_HPP
#define ALL_OBJECTS_HPP

#include "Util/GameObject.hpp"
#include <memory>

class AllObjects : public Util::GameObject {
public:
  virtual ~AllObjects() = default;

  virtual void SetObjectId(int newId) { m_ObjectId = newId; }
  int GetObjectId() const { return m_ObjectId; }

protected:
  // Using protected constructor so it can't be instantiated directly
  AllObjects(int initialId = 0);

  // Forward constructor to GameObject to handle Drawable and ZIndex
  AllObjects(const std::shared_ptr<Core::Drawable> &drawable,
             const float zIndex, int initialId = 0);

  int m_ObjectId = 0;
};

#endif // ALL_OBJECTS_HPP
