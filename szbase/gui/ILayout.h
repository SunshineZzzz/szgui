#pragma once

#include "Common.h"

namespace sz_gui 
{
    namespace layout 
    {
        // 锚点布局
        enum class AnchorPoint
        {
            // 铺满
            Fill,
            // 父容器的左上角
            TopLeft,
            // 父容器的右上角
            TopRight,
            // 父容器的左下角
            BottomLeft,
            // 父容器的右下角
            BottomRight,
            // 父容器的上边缘中点
            TopCenter,
            // 父容器的下边缘中点
            BottomCenter,
            // 父容器的左边缘中点
            CenterLeft,
            // 父容器的右边缘中点
            CenterRight,
            // 父容器的绝对中心点
            Center,
            // 数量
            Count,
        };

        // 边距
        struct Margins
        {
            // 四个方向的边距
            float m_left, m_top, m_right, m_bottom;
            // 标记哪些边距使用百分比
            bool m_isPercentage[4] = { false };

            Margins() : m_left(0.0f), m_top(0.0f), m_right(0.0f), m_bottom(0.0f) {}
            explicit Margins(float all) : m_left(all), m_top(all), m_right(all), m_bottom(all) {}
            Margins(float l, float t, float r, float b) : m_left(l), m_top(t), m_right(r), m_bottom(b) {}

            static Margins Percentage(float lPct, float tPct, float rPct, float bPct)
            {
                if (lPct < 0.0f || lPct > 100.0f || tPct < 0.0f || tPct > 100.0f ||
                    rPct < 0.0f || rPct > 100.0f || bPct < 0.0f || bPct > 100.0f)
                {
                    return Margins();
                }

                Margins m(lPct, tPct, rPct, bPct);

                for (int i = 0; i < 4; ++i)
                {
                    m.m_isPercentage[i] = true;
                }
                return m;
            }

            static Margins Mixed(float l, bool lIsPct, float t, bool tIsPct,
                float r, bool rIsPct, float b, bool bIsPct)
            {
                if ((lIsPct && (l < 0.0f || l > 100.0f)) ||
                    (tIsPct && (t < 0.0f || t > 100.0f)) ||
                    (rIsPct && (r < 0.0f || r > 100.0f)) ||
                    (bIsPct && (b < 0.0f || b > 100.0f)))
                {
                    return Margins();
                }

                Margins m(l, t, r, b);
                m.m_isPercentage[0] = lIsPct;
                m.m_isPercentage[1] = tIsPct;
                m.m_isPercentage[2] = rIsPct;
                m.m_isPercentage[3] = bIsPct;
                return m;
            }
        };
    }

    // 前置声明
    class IUIBase;

    // 布局抽象
	class ILayout
	{
	public:
		ILayout() = default;
		virtual ~ILayout() = default;

		ILayout(const ILayout&) = delete;
		ILayout& operator=(const ILayout&) = delete;

		ILayout(ILayout&&) = delete;
		ILayout& operator=(ILayout&&) = delete;

		// 设置父容器边界
		virtual void SetParentRect(const Rect&) = 0;
		// 添加需要布局的控件
		virtual bool AddWidget(std::shared_ptr<IUIBase> widget) = 0;
        // 删除布局控件
        virtual bool DelWidget(std::shared_ptr<IUIBase> widget) = 0;
        // 执行布局计算
        virtual void PerformLayout() = 0;
	};
}