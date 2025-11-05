// comment: 按钮控件

#pragma once

#include <string>
#include <cstdint>

#include "../UIBase.h"

namespace sz_gui
{
    namespace widget
    {
        // 按钮状态
        enum class ButtonState
        {
            // 禁用状态
            disable,
            // 正常状态
            normal,
            // 鼠标悬停状态，高亮
            hover,
            // 按下状态
            press,
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

        USING_BITMASK_OPERATORS()
    }
}

ENABLE_BITMASK_OPERATORS(sz_gui::widget::ButtonTextAlign)

namespace sz_gui 
{
    namespace widget
    {
        class UIButton : public UIBase
        {
        public:
            // 锚点布局构造
            UIButton(std::string name, layout::AnchorPoint type, layout::Margins margins, 
                uint32_t desiredW, uint32_t desiredH, float desiredZ);

        public:
            // 鼠标点击事件，返回false将会阻止冒泡
            bool OnMouseButton(const events::MouseButtonEventData& data) override;
            // 收集渲染数据事件
            bool OnCollectRenderData() override;
            // 设置颜色主题
            void SetColorTheme(ColorTheme theme) override;

        protected:
            // 按钮颜色
            std::vector<float> m_colors =
            {
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,

				1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
            };
            // 按钮文本
            std::string m_text;
            // 当前按钮状态 
            ButtonState m_state = ButtonState::normal;
            // 按钮文本对其方式
            ButtonTextAlign m_Textlign = ButtonTextAlign::h_center|ButtonTextAlign::v_center;
        };
    }
}