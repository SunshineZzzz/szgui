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
            Disable,
            // 正常状态
            Normal,
            // 鼠标悬停状态，高亮
            Hover,
            // 按下状态
            Press,

            Count,
        };
    }
}

namespace sz_gui 
{
    namespace widget
    {
        class UIButton : public UIBase
        {
        public:
            // 锚点布局构造
            UIButton(std::string name, layout::AnchorPoint type, layout::Margins margins, 
                uint32_t desiredW, uint32_t desiredH);

        public:
            // 鼠标左键点击事件，返回false将会阻止冒泡
            bool OnMouseLeftButtonClick() override;
            // 鼠标左键按下事件
            void OnMouseLeftButtonDown() override;
            // 鼠标左键抬起事件
            void OnMouseLeftButtonUp() override;
            // 鼠标移动进入事件
            void OnMouseMoveEnter() override;
            // 鼠标移动事件
            void OnMouseMove() override;
            // 鼠标移动离开事件
            void OnMouseMoveLeave() override;
            // 收集渲染数据事件
            bool OnCollectRenderData() override;
            // 设置颜色主题
            void SetColorTheme(ColorTheme theme) override;

        private:
            // 设置按钮状态
            void setState(ButtonState state);
            // 加入文字渲染数据
            void appendTextDrawData(UploadOperation uploadOp);

        protected:
            // 按钮颜色
            std::map<ButtonState, std::vector<float>> m_colors;
            // 按钮文本
            std::string m_text = "按钮";
            // 当前按钮状态 
            ButtonState m_state = ButtonState::Normal;
            // 鼠标左键是否按下
            bool m_mouseLeftIsDown = false;
        };
    }
}