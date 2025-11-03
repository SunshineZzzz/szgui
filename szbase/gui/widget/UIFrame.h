// comment: 边框控件

#pragma once

#include <string>
#include <cstdint>

#include "../UIBase.h"
#include "../UIData.h"

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
            // 设置边框宽度
            bool SetBorderWidth(float width) 
            {
                auto& render = m_uiManager.lock()->GetRender();
                auto [min, max] = render->GetLineWidthRange();
                if (width < min || width > max)
				{
					return false;
				}
                m_uiData.m_borderWidth = width;
                return true;
            }
            // 获取边框宽度
            float GetBorderWidth() const { return m_uiData.m_borderWidth; }

        public:
            // 鼠标点击事件，返回false将会阻止冒泡
            bool OnMouseButton(const events::MouseButtonEventData& data) override;
            // 窗户需要重绘事件
            void OnWindowRedraw() override;
            // 收集渲染数据事件
            void OnCollectRenderData() override;

        private:
            // 布局
            std::unique_ptr<ILayout> m_layout;
            // 数据
            FrameUIData m_uiData;
        };
    }
}