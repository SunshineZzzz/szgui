#pragma once

#include <cstdint>
#include <map>

#include "IUIManager.h"
#include "EventTypes.h"
#include "Common.h"

namespace sz_gui 
{
	// 前置声明
	class IUIManager;

	// UI抽象
	class IUIBase
	{
	public:
		IUIBase() = default;
		virtual ~IUIBase() = default;

		IUIBase(const IUIBase&) = delete;
		IUIBase& operator=(const IUIBase&) = delete;

		IUIBase(IUIBase&&) = delete;
		IUIBase& operator=(IUIBase&&) = delete;

		// 设置UI管理器
		virtual void SetUIManager(const std::weak_ptr<IUIManager>&) = 0;
		// 设置父组件
		virtual void SetParent(const std::weak_ptr<IUIBase>&) = 0;
		// 获取父组件
		virtual const std::weak_ptr<IUIBase>& GetParent() const = 0;
		// 添加子组件
		virtual bool addChild(const std::shared_ptr<IUIBase>&) = 0;
		// 移除子组件
		virtual bool removeChild(const std::shared_ptr<IUIBase>&) = 0;
		// 获取所有孩子，按照Z值由小到大排序，Z值一样按照创建顺序排序
		virtual const ChildMultimap& getChilds() const = 0;
		// 获取对应在父组件中的ID
		virtual uint64_t GetChildIdForUIBase() const = 0;
		// 获取对应在UI管理器中的ID
		virtual uint64_t GetChildIdForUIManager() const = 0;
		// 设置在父组件中的ID
		virtual void setChildIdForUIBase(uint64_t) = 0;
		// 设置在UI管理器中的ID
		virtual void setChildIdForUIManager(uint64_t) = 0;
		// 判断点是否在组件内
		virtual bool ContainsPoint(float, float) const = 0;
		// 获取UI的ZValue
		virtual float GetZValue() const = 0;
		// 鼠标点击事件，返回false将会阻止冒泡
		virtual bool OnMouseButton(const events::MouseButtonEventData&) = 0;
	};
}