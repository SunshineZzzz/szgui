#include "UIBase.h"

namespace sz_gui
{
	void UIBase::SetUIManager(const std::weak_ptr<IUIManager>& uiManger)
	{
		assert(m_uiManager.expired());
		m_uiManager = uiManger;
	}

	void UIBase::SetParent(const std::weak_ptr<IUIBase>& parent)
	{
		bool ok = false;
		if (!parent.expired())
		{
			// 要设置的父亲存在
			if (!m_parent.expired())
			{
				// 已经有父亲，脱离
				ok = m_parent.lock()->removeChild(shared_from_this());
				if (!ok) [[unlikely]] assert(0);
				m_parent = parent;
				ok = parent.lock()->addChild(shared_from_this());
				if (!ok) [[unlikely]] assert(0);
			}
			else 
			{
				// 没有父亲，直接设置
				m_parent = parent;
				ok = parent.lock()->addChild(shared_from_this());
				if (!ok) [[unlikely]] assert(0);
				// 还要加入到全局
				ok = m_uiManager.lock()->RegUI(shared_from_this());
				if (!ok) [[unlikely]] assert(0);
			}
		}

		// 要设置的父亲不存在
		if (!m_parent.expired())
		{
			// 已经有父亲，脱离
			ok = m_parent.lock()->removeChild(shared_from_this());
			if (!ok) [[unlikely]] assert(0);
			ok = m_uiManager.lock()->UnRegUI(shared_from_this());
			if (!ok) [[unlikely]] assert(0);
			m_parent.reset();
		}
	}

	bool UIBase::addChild(const std::shared_ptr<IUIBase>& child)
	{
		if (!child)
		{
			return false;
		}

		if (m_childUnorderedmap.find(child->GetChildIdForUIBase()) != m_childUnorderedmap.end())
		{
			return false;
		}

		const auto id = GenChildId();
		child->setChildIdForUIBase(id);
		m_childUnorderedmap[id] = m_childMultimap.insert({std::pair(id, child->GetZValue()), child });
		return true;
	}

	bool UIBase::removeChild(const std::shared_ptr<IUIBase>& child)
	{
		if (!child || child->GetChildIdForUIBase() == 0)
		{
			return false;
		}

		if (m_childUnorderedmap.find(child->GetChildIdForUIBase()) == m_childUnorderedmap.end())
		{
			return false;
		}

		auto& it = m_childUnorderedmap[child->GetChildIdForUIBase()];
		if (m_childMultimap.find(it->first) == m_childMultimap.end())
		{
			return false;
		}

		m_childMultimap.erase(it);
		m_childUnorderedmap.erase(child->GetChildIdForUIBase());
		child->setChildIdForUIBase(0);
		return true;
	}

	bool UIBase::ContainsPoint(float x, float y) const
	{
		return (x >= m_x && x <= m_x + m_width &&
			y >= m_y && y <= m_y + m_height);
	}
}