#pragma once

#include <cassert>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <map>
#include <string>

#include "IUIBase.h"
#include "IRender.h"
#include "../ds/EventBus.h"

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
		// 获取UI管理器
		const std::weak_ptr<IUIManager>& GetUIManager() const override { return m_uiManager; }
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
		// 获取宽高
		float GetWidth() const override { return m_width; };
		float GetHeight() const override { return m_height; };
		// 获取矩形
		const sz_ds::Rect GetRect() const override { return sz_ds::Rect{ m_x, m_y, m_width, m_height }; }
		// 设置矩形
		void SetRect(const sz_ds::Rect& rect)
		{
			m_x = rect.m_x;
			m_y = rect.m_y;
			m_width = rect.m_width;
			m_height = rect.m_height;
		}
		// 获取坐标
		const glm::vec3 GetPos() const override { return glm::vec3{ m_x, m_y, m_z }; }
		// 获取AnchorPoint
		layout::AnchorPoint GetAnchorPoint() const override { return m_anchorPoint; };
		// 获取边距
		layout::Margins GetMargins() const override  { return m_margins; };
		// 鼠标点击事件，返回false将会阻止冒泡
		bool OnMouseButton(const events::MouseButtonEventData&) override { return false; };
		// 窗户大小发生改变事件
		void OnWindowSizeChange() override { return; };
		// 收集渲染数据事件
		void OnCollectRenderData() override { return; };
		// 获取名称
		const std::string& GetName() override 
		{ 
			if (m_fullName.empty())
			{
				m_fullName = m_name + std::to_string(GetChildIdForUIManager());
			}
			return m_fullName;
		};
		// 设置UI标记
		void SetUIFlag(UIFlag flag) override  { m_flag |= flag; }
		// 是否有UI标记
		bool HasUIFlag(UIFlag flag) const override { return HasFlag(m_flag, flag); }
		// 设置2d纹理单元Id
		virtual void SetTexture2dUnitId(uint32_t id) override { m_texture2dUintId = id; }
		// 设置使用颜色
		virtual void SetUseColor(bool bUse) override { m_useColor = bUse; };
		// 设置shaderId
		virtual void SetShaderId(uint32_t id) override { m_shaderId = id; };
		// 设置颜色
		virtual void SetColor(sz_ds::Rgba4Byte color) override { m_color = color; }
		// 获取当前UI和父UI的AABB2D交集
		sz_ds::AABB2D getIntersectWithParent() const override;
		// 标记为脏
		void MarkDirty() override { m_dirty = true; };
		// 清除脏标记
		void ClearDirty() override { m_dirty = false; };
		// 检查是否为脏
		bool IsDirty() const override { return m_dirty; };

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
		float m_z = 0.0f;		
		float m_width = 0.0f;
		float m_height = 0.0f;
		// 边距
		layout::Margins m_margins;
		// 锚点布局类型
		layout::AnchorPoint m_anchorPoint = layout::AnchorPoint::Center;
		// 初始为true，首次需要渲染
		bool m_dirty = true;
		// 事件总线
		sz_ds::EventBus m_eventbus;
		// 名称
		std::string m_name;
		// 全名称
		std::string m_fullName;
		// 标记
		UIFlag m_flag = UIFlag::Visibale;
		// UI类型
		UIType m_type = UIType::None;
		// 2d纹理单元Id
		uint32_t m_texture2dUintId = 0;
		// shaderId
		uint32_t m_shaderId = 0;
		// 颜色
		sz_ds::Rgba4Byte m_color;
		// 是否使用颜色
		bool m_useColor= false;
	};
}