#pragma once

#include "../IUIBase.h"
#include "../ILayout.h"

#include <memory>
#include <list>
#include <unordered_map>
#include <cstdint>

namespace sz_gui 
{
	namespace layout 
	{
        // 锚点布局项
        struct AnchorLayoutItem 
        {
            // 控件
            std::shared_ptr<IUIBase> m_widget;
            // 锚点
            AnchorPoint anchor;
            // 边距
            Margins margins;
            // 计算后的实际位置和大小
            Rect calculatedRect;
        };

        // 锚点布局
        class AnchorLayout final: public ILayout
        {
        private:
            // 父容器矩形
            Rect m_parentRect;
            // 需要布局的控件
            std::list<AnchorLayoutItem> m_items;
            // 记录需要布局的控件
            std::unordered_map<uint64_t, std::list<AnchorLayoutItem>::iterator> m_records;

        public:
            // 设置父容器边界
            void SetParentRect(const Rect& rect) override
            {
                m_parentRect = rect;
            }
            // 添加需要布局的控件
            bool AddWidget(std::shared_ptr<IUIBase> widget) override
            {
                if (!widget || widget->GetChildIdForUIManager() == 0)
				{
					return false;
				}

                if (m_records.find(widget->GetChildIdForUIManager()) != m_records.end())
				{
					return false;
				}
                
                m_items.emplace_back(widget, widget->GetAnchorPoint(), widget->GetMargins(), Rect());
                auto it = std::prev(m_items.end());
                m_records.emplace(widget->GetChildIdForUIManager(), it);
                return true;
            }
            // 删除布局控件
            bool DelWidget(std::shared_ptr<IUIBase> widget) override
            {
                if (!widget || widget->GetChildIdForUIManager() == 0)
                {
                    return false;
                }
                
                if (m_records.find(widget->GetChildIdForUIManager()) == m_records.end())
                {
                    return false;
                }

                m_items.erase(m_records[widget->GetChildIdForUIManager()]);
                m_records.erase(widget->GetChildIdForUIManager());
                return true;
            }
            // 执行布局计算
            void PerformLayout() override
            {
                for (auto& item : m_items) 
                {
                    item.calculatedRect = calculateItemRect(item);
                    item.m_widget->SetRect(item.calculatedRect);
                }
            }

        private:
            // 计算实际边距值
            Margins calculateActualMargins(const Margins& margins, float parentWidth, float parentHeight) const
            {
                Margins actual;

                actual.m_left = margins.m_isPercentage[0] ? (margins.m_left * 0.01f * parentWidth) : margins.m_left;
                actual.m_top = margins.m_isPercentage[1] ? (margins.m_top * 0.01f * parentHeight) : margins.m_top;
                actual.m_right = margins.m_isPercentage[2] ? (margins.m_right * 0.01f * parentWidth) : margins.m_right;
                actual.m_bottom = margins.m_isPercentage[3] ? (margins.m_bottom * 0.01f * parentHeight) : margins.m_bottom;

                return actual;
            }
            // 计算控件矩形
            Rect calculateItemRect(const AnchorLayoutItem& item) const;
        };
	}
}