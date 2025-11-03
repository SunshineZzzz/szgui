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

    void UIManager::Init(int width, int height)
    {
        m_width = width;
        m_height = height;
    }

    void UIManager::RunBeforWork()
    {
        // 重新布局
        m_layout->SetParentRect({ 0.0f, 0.0f, (float)m_width, (float)m_height });
        m_layout->PerformLayout();
        // 子组件重新布局
        for (auto& it : m_topUIMultimap)
        {
            it.second->OnWindowResize();
        }
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
        topUI->SetUIManager(shared_from_this());
        m_topUIUnorderedmap[id] = m_topUIMultimap.insert({std::make_pair(id, topUI->GetZValue()), topUI });
        if (!RegUI(topUI)) [[unlikely]] assert(0);

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
        topUI->SetUIManager(std::weak_ptr<IUIManager>());
        if (!UnRegUI(topUI)) [[unlikely]] assert(0);
        
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

        if (m_allNameUIUnorderedmap.find(ui->GetName()) != m_allNameUIUnorderedmap.end())
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
        m_allNameUIUnorderedmap[ui->GetName()] = id;
        
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
        if (m_allNameUIUnorderedmap.find(ui->GetName()) == m_allNameUIUnorderedmap.end())
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
        m_allNameUIUnorderedmap.erase(ui->GetName());
        ui->setChildIdForUIManager(0);
 
        return true;
    }

	bool UIManager::HandleEvent(std::any eventContainer)
	{
		const SDL_Event* event = std::any_cast<SDL_Event*>(eventContainer);
        if (!event)
		{
			return false;
		}

        switch (event->type)
        {
        case SDL_EVENT_QUIT:
        {
        }
        break;
        case SDL_EVENT_WINDOW_RESIZED:
        {
            m_width = event->window.data1;
            m_height = event->window.data2;

            // 重新布局
            m_layout->SetParentRect({ 0.0f, 0.0f, (float)m_width, (float)m_height });
            m_layout->PerformLayout();
            // 子组件重新布局
            for (auto& it : m_topUIMultimap)
            {
                it.second->OnWindowResize();
            }
        }
        break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            if (event->button.button != SDL_BUTTON_LEFT && 
                event->button.button != SDL_BUTTON_RIGHT &&
                event->button.button != SDL_BUTTON_MIDDLE)
            {
                return false;
            }
            // 鼠标点击事件
            bubbleEvent(*event);
        }
        break;
        default:
            return false;
        }
        return true;
	}

    void UIManager::Render()
    {
        assert(m_topUIMultimap.size() == m_topUIUnorderedmap.size());
        assert(m_allUIMultimap.size() == m_allUIUnorderedmap.size());
        assert(m_allNameUIUnorderedmap.size() == m_allUIUnorderedmap.size());

        for (auto& it : m_topUIMultimap)
        {
            it.second->OnCollectRenderData();
        }

        // 渲染所有UI组件
        m_render->Render();
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