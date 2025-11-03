#include "UIButton.h"

namespace sz_gui
{
	namespace widget
	{
		UIButton::UIButton(std::string name, layout::AnchorPoint type, layout::Margins margins, 
			uint32_t desiredW, uint32_t desiredH, float desiredZ)
		{
			m_type = UIType::Button;

			m_name = std::move(name);
			m_anchorPoint = type;
			m_margins = std::move(margins);
			m_width = (float)desiredW;
			m_height = (float)desiredH;
			m_z = desiredZ;
		}

		bool UIButton::OnMouseButton(const events::MouseButtonEventData& data)
		{
			// m_eventbus.Publish<events::MouseButtonEvent>(data);
			return false;
		}

		void UIButton::OnCollectRenderData()
		{

		}
	}
}