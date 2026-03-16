#ifndef NOTICE_UI_HPP
#define NOTICE_UI_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <memory>

class NoticeUI : public Util::GameObject {
public:
    NoticeUI();
    void SetVisible(bool visible);

private:
    std::shared_ptr<Util::Image> m_image;
};

#endif // NOTICE_UI_HPP
