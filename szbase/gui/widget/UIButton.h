// comment: 按钮控件

#pragma once

#include <string>
#include <cstdint>

#include "../UIBase.h"

namespace sz_gui 
{
    namespace widget
    {
        class UIButton : public UIBase
        {
        public:
            // 按钮状态
            enum class ButtonState
            {
                // 禁用状态
                disable,
                // 正常状态
                normal,
                // 鼠标悬停状态，高亮
                highlight,
                // 按下状态
                press,

                count,
            };

            // 文本对齐方式
            enum class ButtonTextAlign : uint32_t
            {
                h_left = 1 << 0,
                h_right = 1 << 1,
                h_center = 1 << 2,
                v_top = 1 << 3,
                v_bottom = 1 << 4,
                v_center = 1 << 5,
            };
        public:
            // 锚点布局构造
            UIButton(std::string name, layout::AnchorPoint type, layout::Margins margins, 
                uint32_t desiredW, uint32_t desiredH, float desiredZ);
            // 鼠标点击事件，返回false将会阻止冒泡
            bool OnMouseButton(const events::MouseButtonEventData& data) override;

        protected:
            // 按钮文本
            std::string m_text;
            // 当前按钮状态
            ButtonState m_status = ButtonState::normal;
            // 按钮文本对其方式
            uint32_t m_Textlign = (uint32_t)ButtonTextAlign::h_center | (uint32_t)ButtonTextAlign::v_center;
        };
    }
}