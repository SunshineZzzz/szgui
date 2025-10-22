#include "UIFrame.h"

#include <glm/glm.hpp>

namespace sz_gui
{
	namespace widget
	{
		UIFrame::UIFrame(std::string name, layout::AnchorPoint type, layout::Margins margins, 
			uint32_t desiredW, uint32_t desiredH, float desiredZ)
		{
			m_name = std::move(name);
			m_anchorPoint = type;
			m_margins = std::move(margins);
			m_width = (float)desiredW;
			m_height = (float)desiredH;
			m_z = desiredZ;
		}

		bool UIFrame::OnMouseButton(const events::MouseButtonEventData& data)
		{
			// m_eventbus.Publish<events::MouseButtonEvent>(data);
			return false;
		}

		void UIFrame::OnWindowSizeChange()
		{
			// ���²���
			m_layout->SetParentRect(GetRect());
			m_layout->PerformLayout();

			// �����ռ���Ⱦ����
			OnCollectRenderData();
			return;
		}

		void UIFrame::OnCollectRenderData()
		{
			glm::vec3 pos = GetPos();
			sz_ds::AABB2D aabb(m_x, m_y, m_x + m_width, m_y + m_height);

			if (!HasUIFlag(UIFlag::Top))
			{
				pos += m_parent.lock()->GetPos();

				aabb = getIntersectWithParent();
				if (!aabb.IsNull())
				{
					return;
				}
			}

			// ������Ⱦ����
			sz_ds::Vertex vertex[] =
			{
				{
					// ���Ͻ�
					{m_x, m_y, m_z},
					// ��ͼ��������Ͻ�
					{m_uvLT.x, m_uvLT.y, 0.0f},
					{m_color.m_r, m_color.m_g, m_color.m_b, m_color.m_a},
				},
				{
					// ���Ͻ�
					{m_x + m_width, m_y, m_z},
					// ��ͼ��������Ͻ�
					{m_uvRB.x, m_uvLT.y, 0.0f},
					{m_color.m_r, m_color.m_g, m_color.m_b, m_color.m_a},
				},
				{
					// ���½�
					{m_x + m_width, m_y + m_height, m_z},
					// ��ͼ��������½�
					{m_uvLT.x,m_uvRB.y, 0.0f},
					{m_color.m_r, m_color.m_g, m_color.m_b, m_color.m_a},
				},
				{
					// ���½�
					{m_x, m_y + m_height, m_z},
					// ��ͼ��������½�
					{m_uvLT.x, m_uvRB.y, 0.0f},
					{m_color.m_r, m_color.m_g, m_color.m_b, m_color.m_a},
				},
			};

			// ������������
			unsigned int indices[] = { 0, 1, 3, 4 };


		}
	}
}