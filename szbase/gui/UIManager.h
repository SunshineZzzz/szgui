#pragma once

#include <vector>
#include <cassert>
#include <cstdint>
#include <string>
#include <set>

#include "IRender.h"
#include "IUIManager.h"
#include "IUIBase.h"
#include "EventTypes.h"
#include "Common.h"
#include "ILayout.h"
#include "InputControl.h"

namespace sz_gui 
{
	// UI管理器
	class UIManager: public IUIManager, public std::enable_shared_from_this<UIManager>
	{
	public:
		UIManager(std::shared_ptr<IRender> render);
		~UIManager();

		// 生成UIID
		const uint64_t GenUIId()
		{
			if (m_nextUIId == 0) [[unlikely]]
			{
				m_nextUIId = 1;
			}
			auto id = m_nextUIId++;
			if (m_topUIUnorderedmap.find(id) != m_topUIUnorderedmap.end()) [[unlikely]]
			{
				assert(0);
			}
			return id;
		}

	public:
		// 初始化
		void Init(int width, int height) override;
		// 运行前工作
		void RunBeforWork() override;
		// 获取render
		const std::shared_ptr<IRender>& GetRender() const override
		{
			return m_render;
		}
		// 注册顶层UI
		bool RegTopUI(std::shared_ptr<IUIBase> topUI) override;
		// 注销顶层UI
		bool UnRegTopUI(std::shared_ptr<IUIBase> topUI) override;
		// 注册UI
		bool RegUI(std::shared_ptr<IUIBase> ui) override;
		// 注销UI
		bool UnRegUI(std::shared_ptr<IUIBase> ui) override;
		// 处理事件
		bool HandleEvent(std::any eventContainer) override;
		// 设置布局
		void SetLayout(ILayout* layout) override 
		{ 
			m_layout.reset(layout); 
		};
		// 布局添加widget
		bool LayoutAddWidget(std::shared_ptr<IUIBase> widget) override { return m_layout->AddWidget(widget); }
		// 布局移除widget
		bool LayoutDelWidget(std::shared_ptr<IUIBase> widget) override { return m_layout->DelWidget(widget); }
		// 绘制
		void Render() override;
		// 获取输入控制
		const InputControl* GetInputControl() const override { return &m_inputControl; };

	private:
		// 根据位置填充UI链
		bool findTargetWriteChainAtPoint(const std::shared_ptr<IUIBase>& findChild,
			std::vector<std::weak_ptr<IUIBase>>& chain);
		// 事件冒泡
		void bubbleEvent(const std::shared_ptr<IUIBase>& findChild);
		// 触发鼠标点击事件
		void triggerMouseButton(const std::vector<std::weak_ptr<IUIBase>>& propagationChain);
		// 鼠标左键事件
		void mouseLeftButtonEvent();
		// 鼠标移动事件
		void mouseMoveEvent();

	private:
		// 渲染器
		std::shared_ptr<IRender> m_render = nullptr;
		// 顶层UI组件，按照Z值由小到大排序，Z值一样按照创建顺序排序
		TopChildMultimap m_topUIMultimap;
		// 记录顶层UI组件Id <-> 顶层UI组件迭代器
		TopChildUnorderedmap m_topUIUnorderedmap;
		// 所有UI组件Id <-> UI组件
		AllUIMultimap m_allUIMultimap;
		// 记录所有UI组件Id <-> UI组件迭代器
		AllChildUnorderedmap m_allUIUnorderedmap;
		// 记录所有UI组件名称 <-> UIId
		std::unordered_map<std::string, uint64_t> m_allNameUIUnorderedmap;
		// UIID生成器
		uint64_t m_nextUIId = 1;
		// 窗口宽高
		int m_width = 0;
		int m_height = 0;
		// 布局
		std::unique_ptr<ILayout> m_layout;
		// 鼠标移动进入UI
		std::shared_ptr<IUIBase> m_mouseMoveEnterUI;
		// 鼠标左键按下UI
		std::shared_ptr<IUIBase> m_mouseLeftPressUI;
		// 输入控制
		InputControl m_inputControl;
	};
}