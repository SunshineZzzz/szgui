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


        public:
            // 鼠标点击事件，返回false将会阻止冒泡
            bool OnMouseButton(const events::MouseButtonEventData& data) override;
            // 窗户大小改变事件
            void OnWindowResize() override;
            // 收集渲染数据事件
            bool OnCollectRenderData() override;
            // 设置颜色主题
            void SetColorTheme(ColorTheme theme) override;

        private:
            // 布局
            std::unique_ptr<ILayout> m_layout;
            static constexpr float m_borderWidth = 1.0f;
            // 顶点颜色信息
            std::vector<float> m_colors =
            {
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
            };
        };
    }
}