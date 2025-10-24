// comment: 正交投影摄像机

#pragma once

#include "Camera.h"

namespace sz_gui
{
	namespace gles
	{
		class OrthographicCamera :public Camera
		{
		public:
			OrthographicCamera(float l, float r, float t, float b, float n, float f);
			virtual ~OrthographicCamera() override; ;

			// 重置投影盒子
			void Reset(float l, float r, float t, float b, float n, float f)
			{
				m_left = l;
				m_right = r;
				m_top = t;
				m_bottom = b;
				m_near = n;
				m_far = f;
			}

			// 获取投影矩阵
			glm::mat4 GetProjectionMatrix() override;
			// 摄像机缩放
			void Scale(float deltaScale) override;

		private:
			// 投影盒子定义
			// 左边界
			float m_left = 0.0f;
			// 右边界
			float m_right = 0.0f;
			// 上边界
			float m_top = 0.0f;
			// 下边界
			float m_bottom = 0.0f;
			// 近平面
			float m_near = 0.0f;
			// 远平面
			float m_far = 0.0f;
			// 摄像机缩放比例
			float m_scale{ 0.0f };
		};
	}
}