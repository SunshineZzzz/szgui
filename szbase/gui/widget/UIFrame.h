// comment: 边框控件

#pragma once

#include <string>
#include <cstdint>

#include "../UIBase.h"

namespace sz_gui 
{
    namespace widget
    {
        class UIFrame : public UIBase
        {
        public:
            // 锚点布局构造
            UIFrame(std::string name, layout::AnchorPoint type, layout::Margins margins, 
                uint32_t desiredW, uint32_t desiredH, float desireZ);

            // 设置布局
            void SetLayout(ILayout* layout) { m_layout.reset(layout); };
            // 添加需要布局的控件
            bool AddWidget(std::shared_ptr<IUIBase> widget) 
            {
                if (!m_layout)
                {
                    return false;
                }
                return m_layout->AddWidget(widget);
            }
            // 删除布局控件
            bool DelWidget(std::shared_ptr<IUIBase> widget)
            {
                if (!m_layout)
                {
                    return false;
                }
                return m_layout->DelWidget(widget);
            }

        private:
            // 布局
            std::unique_ptr<ILayout> m_layout;
        };
    }
}