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
		// 设置UI的ZValue
		void SetZValue(float z) override { m_z = z; }
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
		// 获取期望宽高
		std::tuple<float, float> GetDisireWH() const override { return { m_desireWidth, m_desireHeight }; }
		// 获取坐标
		const glm::vec3 GetPos() const override { return glm::vec3{ m_x, m_y, m_z }; }
		// 获取AnchorPoint
		layout::AnchorPoint GetAnchorPoint() const override { return m_anchorPoint; };
		// 获取边距
		layout::Margins GetMargins() const override  { return m_margins; };
		// 鼠标左键点击事件，返回false将会阻止冒泡
		bool OnMouseLeftButtonClick() override  { return false; };
		// 鼠标左键按下事件
		void OnMouseLeftButtonDown() override  { return; };
		// 鼠标左键抬起事件
		void OnMouseLeftButtonUp() override  { return; };
		// 窗户发生变化事件
		void OnWindowResize() override 
		{ 
			setUploadOp(UploadOperation::UploadPos);
			setUploadOp(UploadOperation::UploadText);
			return; 
		};
		// 鼠标移动进入事件
		void OnMouseMoveEnter() {};
		// 鼠标移动事件
		void OnMouseMove() {};
		// 鼠标移动离开事件
		void OnMouseMoveLeave() {};
		// 收集渲染数据事件
		bool OnCollectRenderData() override { return false; };
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
		// 清除UI标记
		void ClearUIFlag(UIFlag flag) override { m_flag &= ~flag; }
		// 是否有UI标记
		bool HasUIFlag(UIFlag flag) const override { return HasFlag(m_flag, flag); }
		// 是否可见
		virtual bool IsVisible() const override
		{
			return HasUIFlag(UIFlag::Visibale);
		}
		// 是否可交互
		virtual bool IsInteractive() const override
		{
			return HasUIFlag(UIFlag::Interactive);
		}
		// 获取当前UI和父UI的AABB2D交集
		sz_ds::AABB2D getIntersectWithParent() const override;
		// 设置颜色主题
		void SetColorTheme(ColorTheme theme) override
		{
			m_colorTheme = theme;
			setUploadOp(UploadOperation::UploadColorOrUv);
		}

	protected:
		// 获取上传操作
		UploadOperation getUploadOp()  
		{
			auto op = m_uploadOp;
			m_uploadOp = UploadOperation::Retain;
			return op;
		}
		// 设置上传操作
		void setUploadOp(UploadOperation op) 
		{ 
			m_uploadOp &= ~UploadOperation::Retain;
			m_uploadOp |= op;
		}

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
		// 期望宽高
		float m_desireWidth = 0.0f;
		float m_desireHeight = 0.0f;
		// 边距
		layout::Margins m_margins;
		// 锚点布局类型
		layout::AnchorPoint m_anchorPoint = layout::AnchorPoint::Center;
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
		// 上传操作
		UploadOperation m_uploadOp = UploadOperation::UploadColorOrUv | 
			UploadOperation::UploadPos | UploadOperation::UploadIndex;
		// 颜色主题
		ColorTheme m_colorTheme = ColorTheme::LightMode;
	};
}