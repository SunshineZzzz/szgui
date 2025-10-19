#pragma once

#include <cassert>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <map>

#include "IUIBase.h"

namespace sz_gui 
{
	// UI基类
	class UIBase : public IUIBase, public std::enable_shared_from_this<UIBase>
	{
	public:
		// 生成子ID
		const uint64_t GenChildId() 
		{
			if (m_nextChildId == 0) [[unlikely]]
			{
				m_nextChildId = 1;
			}
			auto id = m_nextChildId++;
			if (m_childUnorderedmap.find(id) != m_childUnorderedmap.end()) [[unlikely]]
			{
				assert(0);
			}
			return id;
		}

	public:
		// 设置UI管理器
		void SetUIManager(const std::weak_ptr<IUIManager>& uiManger) override;
		// 设置父组件
		void SetParent(const std::weak_ptr<IUIBase>& parent) override;
		// 获取父组件
		const std::weak_ptr<IUIBase>& GetParent() const override { return m_parent; }
		// 获取对应在父组件中的ID
		uint64_t GetChildIdForUIBase() const override { return m_childIdForUIBase; }
		// 获取对应在UI管理器中的ID
		uint64_t GetChildIdForUIManager() const override { return m_childIdForUIManager; }
		// 设置在父组件中的ID
		virtual void setChildIdForUIBase(uint64_t id) { m_childIdForUIBase = id; }
		// 设置在UI管理器中的ID
		virtual void setChildIdForUIManager(uint64_t id) { m_childIdForUIManager = id; }
		// 添加子组件
		bool addChild(const std::shared_ptr<IUIBase>& child);
		// 移除子组件
		bool removeChild(const std::shared_ptr<IUIBase>& child);
		// 获取所有孩子，按照Z值由小到大排序，Z值一样按照创建顺序排序
		const ChildMultimap& getChilds() const { return m_childMultimap; }
		// 判断点是否在组件内
		bool ContainsPoint(float x, float y) const override;
		// 获取UI的ZValue
		float GetZValue() const override { return m_z; }
		// 鼠标点击事件，返回false将会阻止冒泡
		bool OnMouseButton(const events::MouseButtonEventData&) { return false; };

	protected:
		// UI管理器
		std::weak_ptr<IUIManager> m_uiManager;
		// 父组件
		std::weak_ptr<IUIBase> m_parent;
		// 孩子组件们，按照Z值由小到大排序，Z值一样按照创建顺序排序
		ChildMultimap m_childMultimap;
		// 孩子组件Id <-> 孩子组件迭代器
		ChildUnorderedMap m_childUnorderedmap;
		// ID生成器
		uint64_t m_nextChildId = 1;
		// 子ID
		uint64_t m_childIdForUIBase = 0;
		uint64_t m_childIdForUIManager = 0;
		// 组件位置(相对于窗口)，深度(越大越说明在顶层)，和尺寸
		float m_x = 0.0f;
		float m_y = 0.0f;
		float m_z = 0;		
		float m_width = 0.0f;
		float m_height = 0.0f;
		// 初始为true，首次需要渲染
		bool m_dirty = true;
	};
}