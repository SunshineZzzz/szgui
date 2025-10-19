#pragma once

#include <SDL3/SDL.h>

#include <memory>
#include <map>
#include <unordered_map>
#include <cassert>
#include <cstdint>
#include <string>

#include "IRender.h"
#include "IUIManager.h"
#include "IUIBase.h"
#include "EventTypes.h"
#include "Common.h"

namespace sz_gui 
{
	// UI管理器
	class UIManager: public IUIManager
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
		// 注册顶层UI
		bool RegTopUI(std::shared_ptr<IUIBase> topUI) override;
		// 注销顶层UI
		bool UnRegTopUI(std::shared_ptr<IUIBase> topUI) override;
		// 注册UI
		bool RegUI(std::shared_ptr<IUIBase> ui) override;
		// 注销UI
		bool UnRegUI(std::shared_ptr<IUIBase> ui) override;
		// 处理事件
		bool HandleEvent(const std::any& eventContainer) override;

	private:
		// 根据位置填充UI链
		bool findTargetWriteChainAtPoint(float x, float y, std::vector<std::weak_ptr<IUIBase>>& chain);
		// 事件冒泡
		void bubbleEvent(const SDL_Event& event);
		// 触发鼠标点击事件
		void triggerMouseButton(const events::MouseButtonEventData& mbed,
			const std::vector<std::weak_ptr<IUIBase>>& propagationChain);

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
		// UIID生成器
		uint64_t m_nextUIId = 1;
		// 焦点UI
		std::shared_ptr<IUIBase> m_focusUI = nullptr;
	};
}