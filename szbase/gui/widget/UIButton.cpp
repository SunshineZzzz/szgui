#include "UIButton.h"

namespace sz_gui
{
	namespace widget
	{
		UIButton::UIButton(std::string name, layout::AnchorPoint type, layout::Margins margins, 
			uint32_t desiredW, uint32_t desiredH)
		{
			m_type = UIType::Button;

			m_name = std::move(name);
			m_anchorPoint = type;
			m_margins = std::move(margins);
			m_desireWidth = (float)desiredW;
			m_desireHeight = (float)desiredH;

			SetColorTheme(ColorTheme::LightMode);
		}

		bool UIButton::OnMouseButton(const events::MouseButtonEventData& data)
		{
			// m_eventbus.Publish<events::MouseButtonEvent>(data);
			return false;
		}

		bool UIButton::OnCollectRenderData()
		{
			if (!IsVisible())
			{
				return false;
			}

			auto aabb = getIntersectWithParent();
			if (aabb.IsNull())
			{
				return false;
			}

			// 顶点位置信息索引
			static std::vector<float> positions;
			positions.clear();
			positions =
			{
				// 左上角
				0.0f, 0.0f, 0.0f,
				// 右上角
				m_width, 0.0f, 0.0f,
				// 右下角
				m_width, m_height, 0.0f,
				// 左下角
				0.0f, m_height, 0.0f,
			};
			// 顶点颜色信息
			const std::vector<float>& colors = m_colors;
			// 顶点数据索引
			static std::vector<uint32_t> indices = 
			{
				0, 1, 2, 
				2, 3 ,0, 
			};

			// 绘制命令
			DrawCommand dCmd;
			dCmd.m_onlyId = m_childIdForUIManager;
			dCmd.m_worldPos = { m_x, m_y, m_z };
			dCmd.m_uploadOp = getUploadOp();
			dCmd.m_drawMode = DrawMode::TRIANGLES;

			auto& render = m_uiManager.lock()->GetRender();
			render->AppendDrawData(positions, colors, indices, dCmd);

			return true;
		}

        void UIButton::SetColorTheme(ColorTheme theme)
        {
            UIBase::SetColorTheme(theme);

            // 默认蓝
            const float R_NORM = 0.25f; const float G_NORM = 0.60f; const float B_NORM = 0.90f;
            // 亮蓝 (悬停)
            const float R_HOV = 0.30f; const float G_HOV = 0.65f; const float B_HOV = 0.95f;
            // 暗蓝 (按下)
            const float R_PRS = 0.20f; const float G_PRS = 0.55f; const float B_PRS = 0.85f;
            // 褪色蓝 (禁用)
            const float R_DIS = 0.60f; const float G_DIS = 0.80f; const float B_DIS = 0.95f;

            if (m_state == ButtonState::disable)
            {
                switch (theme)
                {
                case ColorTheme::LightMode:
                    m_colors =
                    {
                        R_DIS, G_DIS, B_DIS, R_DIS, G_DIS, B_DIS,
                        R_DIS, G_DIS, B_DIS, R_DIS, G_DIS, B_DIS,
                    };
                    break;
                }
            }
            else if (m_state == ButtonState::normal)
            {
                switch (theme)
                {
                case ColorTheme::LightMode:
                    m_colors =
                    {
                        R_NORM, G_NORM, B_NORM, R_NORM, G_NORM, B_NORM,
                        R_NORM, G_NORM, B_NORM, R_NORM, G_NORM, B_NORM,
                    };
                    break;
                }
            }
            else if (m_state == ButtonState::hover)
            {
                switch (theme)
                {
                case ColorTheme::LightMode:
                    m_colors =
                    {
                        R_HOV, G_HOV, B_HOV, R_HOV, G_HOV, B_HOV,
                        R_HOV, G_HOV, B_HOV, R_HOV, G_HOV, B_HOV,
                    };
                    break;
                }
            }
            else 
            {
                // press
                switch (theme)
                {
                case ColorTheme::LightMode:
                    m_colors =
                    {
                        R_PRS, G_PRS, B_PRS, R_PRS, G_PRS, B_PRS,
                        R_PRS, G_PRS, B_PRS, R_PRS, G_PRS, B_PRS,
                    };
                    break;
                }
            }
        }
	}
}