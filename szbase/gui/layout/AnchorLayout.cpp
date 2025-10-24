#pragma once

#include "AnchorLayout.h"

#include <algorithm>

namespace sz_gui 
{
	namespace layout 
	{
        sz_ds::Rect AnchorLayout::calculateItemRect(const AnchorLayoutItem& item) const
        {
            sz_ds::Rect result{};

            const Margins pixelMargins = calculateActualMargins(item.margins, m_parentRect.m_width, 
                m_parentRect.m_height);

            // 计算父容器在减去边距后，为控件留下的可用空间
            const float availableWidth = std::max(0.0f, 
                m_parentRect.m_width - pixelMargins.m_left - pixelMargins.m_right);
            const float availableHeight = std::max(0.0f, 
                m_parentRect.m_height - pixelMargins.m_top - pixelMargins.m_bottom);

            // 获取控件的期望尺寸
            sz_ds::Rect desiredSize = item.m_widget->GetRect();

            // 在单点锚定模式下，控件的尺寸是期望尺寸与可用空间中的较小值
            result.m_width = availableWidth;
            if (desiredSize.m_width != 0)
            {
                result.m_width = std::min(desiredSize.m_width, availableWidth);
            }
            result.m_height = availableHeight;
            if (desiredSize.m_height != 0)
            {
                result.m_height = std::min(desiredSize.m_height, availableHeight);
            }
            
            switch (item.anchor)
            {
            case AnchorPoint::Fill:
            {
                // 铺满
                // 填充模式使用所有可用空间
                result.m_x = m_parentRect.m_x + pixelMargins.m_left;
                result.m_y = m_parentRect.m_y + pixelMargins.m_top;

                result.m_width = availableWidth;
                result.m_height = availableHeight;
            }
            break;
            // 单锚点
            case AnchorPoint::TopLeft:
            {
                // 父容器的左上角
                result.m_x = m_parentRect.m_x + pixelMargins.m_left;
                result.m_y = m_parentRect.m_y + pixelMargins.m_top;
            }
            break;
            case AnchorPoint::TopRight:
            {
                // 父容器的右上角
                result.m_x = m_parentRect.m_x + m_parentRect.m_width - result.m_width - pixelMargins.m_right;
                result.m_y = m_parentRect.m_y + pixelMargins.m_top;
            }
            break;
            case AnchorPoint::BottomLeft:
            {
                // 父容器的左下角
                result.m_x = m_parentRect.m_x + pixelMargins.m_left;
                result.m_y = m_parentRect.m_y + m_parentRect.m_height - result.m_height - pixelMargins.m_bottom;
            }
            break;
            case AnchorPoint::BottomRight:
            {
                // 父容器的右下角
                result.m_x = m_parentRect.m_x + m_parentRect.m_width - result.m_width - pixelMargins.m_right;
                result.m_y = m_parentRect.m_y + m_parentRect.m_height - result.m_height - pixelMargins.m_bottom;
            }
            break;
            case AnchorPoint::TopCenter:
            {
                // 父容器的上边缘中点
                result.m_x = m_parentRect.m_x + (m_parentRect.m_width / 2.0f) - (result.m_width / 2.0f) + pixelMargins.m_left - pixelMargins.m_right;
                result.m_y = m_parentRect.m_y + pixelMargins.m_top;
            }
            break;
            case AnchorPoint::BottomCenter:
            {
                // 父容器的下边缘中点
                result.m_x = m_parentRect.m_x + (m_parentRect.m_width / 2.0f) - (result.m_width / 2.0f) + pixelMargins.m_left - pixelMargins.m_right;
                result.m_y = m_parentRect.m_y + m_parentRect.m_height - result.m_height - pixelMargins.m_bottom;
            }
            break;
            case AnchorPoint::CenterLeft:
            {
                // 父容器的左边缘中点
                result.m_x = m_parentRect.m_x + pixelMargins.m_left;
                result.m_y = m_parentRect.m_y + (m_parentRect.m_height / 2.0f) - (result.m_height / 2.0f) + pixelMargins.m_top - pixelMargins.m_bottom;
                break;
            }
            break;
            case AnchorPoint::CenterRight:
            {
                // 父容器的右边缘中点
                result.m_x = m_parentRect.m_x + m_parentRect.m_width - result.m_width - pixelMargins.m_right;
                result.m_y = m_parentRect.m_y + (m_parentRect.m_height / 2.0f) - (result.m_height / 2.0f) + pixelMargins.m_top - pixelMargins.m_bottom;
                break;
            }
            break;
            case AnchorPoint::Center:
            {
                // 父容器的绝对中心点
                result.m_x = m_parentRect.m_x + (m_parentRect.m_width / 2.0f) - (result.m_width / 2.0f) + pixelMargins.m_left - pixelMargins.m_right;
                result.m_y = m_parentRect.m_y + (m_parentRect.m_height / 2.0f) - (result.m_height / 2.0f) + pixelMargins.m_top - pixelMargins.m_bottom;
                break;
            }
            break;
            default:
            {
                // 铺满
                // 填充模式使用所有可用空间
                result.m_x = m_parentRect.m_x + pixelMargins.m_left;
                result.m_y = m_parentRect.m_y + pixelMargins.m_top;

                result.m_width = availableWidth;
                result.m_height = availableHeight;
            }
            break;
            }

            result.m_width = std::max(0.0f, result.m_width);
            result.m_height = std::max(0.0f, result.m_height);

            return result;
        }
	}
}