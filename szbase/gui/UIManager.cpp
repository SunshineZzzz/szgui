#include "UIManager.h"
#include "../ds/EventBus.h"

#include <map>

namespace sz_gui
{
	UIManager::UIManager(std::shared_ptr<IRender> render) :
		m_render(render)
	{ }

	UIManager::~UIManager() 
	{
		m_render = nullptr;
	}

    bool UIManager::RegTopUI(std::shared_ptr<IUIBase> topUI)
	{
        if (!topUI)
		{
			return false;
		}

        if (m_topUIUnorderedmap.find(topUI->GetChildIdForUIManager()) != m_topUIUnorderedmap.end())
		{
			return false;
		}
        
        auto id = GenUIId();
        topUI->setChildIdForUIManager(id);
        m_topUIUnorderedmap[id] = m_topUIMultimap.insert({std::make_pair(id, topUI->GetZValue()), topUI });
        if (RegUI(topUI)) [[unlikely]] assert(0);

        return true;
	}

	bool UIManager::UnRegTopUI(std::shared_ptr<IUIBase> topUI)
	{
        if (!topUI || topUI->GetChildIdForUIManager() == 0)
        {
            return false;
        }

        if (m_topUIUnorderedmap.find(topUI->GetChildIdForUIManager()) == m_topUIUnorderedmap.end())
		{
			return false;
		}
    
        auto& it = m_topUIUnorderedmap[topUI->GetChildIdForUIManager()];
        if (m_topUIMultimap.find(it->first) == m_topUIMultimap.end())
        {
            return false;
        }

        m_topUIMultimap.erase(it);
        m_topUIUnorderedmap.erase(topUI->GetChildIdForUIManager());
        topUI->setChildIdForUIManager(0);
        if (UnRegUI(topUI)) [[unlikely]] assert(0);
        
        return true;
	}

    bool UIManager::RegUI(std::shared_ptr<IUIBase> ui)
    {
        if (!ui)
        {
            return false;
        }

       
        if (m_allUIUnorderedmap.find(ui->GetChildIdForUIManager()) != m_allUIUnorderedmap.end())
        {
            return false;
        }

        auto id = ui->GetChildIdForUIManager();
        if (id == 0)
		{
			id = GenUIId();
			ui->setChildIdForUIManager(id);
		}
        m_allUIUnorderedmap[id] = m_allUIMultimap.insert({ std::make_pair(id, ui->GetZValue()), ui });

        return true;
    }

    bool UIManager::UnRegUI(std::shared_ptr<IUIBase> ui)
    {
        if (!ui || ui->GetChildIdForUIManager() == 0)
        {
            return false;
        }

        if (m_allUIUnorderedmap.find(ui->GetChildIdForUIManager()) == m_allUIUnorderedmap.end())
        {
            return false;
        }
        auto& it = m_allUIUnorderedmap[ui->GetChildIdForUIManager()];
        if (m_allUIMultimap.find(it->first) == m_allUIMultimap.end())
		{
			return false;
		}

        m_topUIMultimap.erase(it);
        m_allUIUnorderedmap.erase(ui->GetChildIdForUIManager());
        ui->setChildIdForUIManager(0);

        return true;
    }

	bool UIManager::HandleEvent(const std::any& eventContainer)
	{
		const SDL_Event& event = std::any_cast<const SDL_Event&>(eventContainer);
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
        {
            // 发布窗口退出事件
            sz_ds::PublishEvent<events::WindowEvent>(
                events::WindowEventData::Type::QUIT
            );
        }
        break;
        case SDL_EVENT_WINDOW_RESIZED:
        // 窗口大小改变
        break;
        case SDL_EVENT_WINDOW_MOVED:
        // 窗口移动
        break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            if (event.button.button != SDL_BUTTON_LEFT && 
                event.button.button != SDL_BUTTON_RIGHT &&
                event.button.button != SDL_BUTTON_MIDDLE)
            {
                return false;
            }
            // 鼠标点击事件
            bubbleEvent(event);
        }
        break;
        default:
            return false;
        }
        return true;
	}

    bool UIManager::findTargetWriteChainAtPoint(float x, float y, std::vector<std::weak_ptr<IUIBase>>& chain)
    {
        chain.resize(0);

        std::weak_ptr<IUIBase> findChilds;
        // m_allUIMultimap，倒叙，按照Z值由大到小排序，Z值一样按照创建顺序排序
        for (auto it = m_allUIMultimap.rbegin(); it != m_allUIMultimap.rend(); ++it)
		{
			if (it->second->ContainsPoint(x, y))
			{
				// 最近者优先
				findChilds = it->second;
				break;
			}
		}

        if (findChilds.expired())
        {
            return false;
        }
        chain.push_back(findChilds);

        auto current = findChilds.lock()->GetParent();
        while (!current.expired())
        {
            if (current.lock()->ContainsPoint(x, y))
            {
                chain.push_back(current);
            }
            current = current.lock()->GetParent();
        }
        return true;
    }

    void UIManager::bubbleEvent(const SDL_Event& event)
    {   
        // 命中情况下，深度+冒泡，规则：最近者(深度最大者)优先
        static std::vector<std::weak_ptr<IUIBase>> propagationChain;
        propagationChain.resize(0);

        auto bFind = findTargetWriteChainAtPoint(event.button.x, event.button.y, propagationChain);
        if (!bFind || propagationChain.empty())
        {
            return;
        }

        switch (event.type)
        {
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            // 鼠标点击事件
            auto t = events::MouseButtonEventData::Type::BUTTON_LEFT;
            if (event.button.button == SDL_BUTTON_RIGHT)
            {
                t = events::MouseButtonEventData::Type::BUTTON_RIGHT;
            }
            else if (event.button.button == SDL_BUTTON_MIDDLE)
            {
                t = events::MouseButtonEventData::Type::BUTTON_MIDDLE;
            }
            else
            {
                return;
            }
            auto state = (event.button.type == SDL_EVENT_MOUSE_BUTTON_DOWN) ?
                events::MouseButtonEventData::State::DOWN : events::MouseButtonEventData::State::UP;
            events::MouseButtonEventData mbed{ t, state, event.button.x, event.button.y };
            triggerMouseButton(mbed, propagationChain);
        }
        break;
        }

        propagationChain.resize(0);
    }

    void UIManager::triggerMouseButton(const events::MouseButtonEventData& mbed,
        const std::vector<std::weak_ptr<IUIBase>>& propagationChain)
    {
        for (auto it = propagationChain.begin(); it != propagationChain.end(); ++it)
        {
            if (it->lock()->OnMouseButton(mbed))
            {
                break;
            }
        }
    }
}