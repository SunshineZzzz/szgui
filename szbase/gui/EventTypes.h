#pragma once

#include <stdint.h>

#include "../ds/EventBus.h"

namespace sz_gui 
{
	namespace events
	{
        // 前置声明
        struct MouseClickEventData;
        using MouseButtonEvent = sz_ds::Event<events::MouseClickEventData*>;

        // 鼠标按键类型
        enum class MouseButtonType
        {
            // 左键
            BUTTON_LEFT,
        };

        // 鼠标点击事件数据
        struct MouseClickEventData
        {
            // 鼠标按键类型
            const MouseButtonType m_type{ MouseButtonType::BUTTON_LEFT };
            // 控件唯一Id
            const uint64_t m_widgetOnlyId{ 0 };
        };
	}
}