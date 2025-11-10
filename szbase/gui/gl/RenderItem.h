// comment: 渲染对象

#pragma once

#ifdef USE_OPENGL_ES
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

#include "../IRender.h"
#include "Geometry.h"

namespace sz_gui
{
	namespace gl
	{
		struct RenderItem
		{
			// 获取模型矩阵
			glm::mat4 GetModelMatrix() const
			{
				return glm::translate(glm::mat4(1.0f), m_position);
			}

			// 世界坐标系位置
			glm::vec3 m_position{ 0.0f };
			// 绘制指令
			GLenum m_drawMode{ GL_TRIANGLES  };
			// 几何
			std::unique_ptr<Geometry> m_geo = nullptr;
			// 材质类型
			MaterialType m_materialType = MaterialType::ColorMaterial;
			// 深度检测相关
			bool m_depthTest{ true };
			GLenum m_depthFunc{ GL_LEQUAL };
			bool m_depthWrite{ true };

			// 颜色混合相关
			bool m_blend{ false };
			// 颜色混合方程
			GLenum m_sFactor{ GL_SRC_ALPHA };
			GLenum m_dFactor{ GL_ONE_MINUS_SRC_ALPHA };
			// 0.0f~1.0f，0.0f完全透明，1.0f完全不透明
			// 作用于物体本身透明度
			float m_opacity{ 1.0f };

			// 面剔除相关
			// 是否开启面剔除
			bool m_faceCulling{ true };
			// 面剔除方式
			GLenum m_frontFace{ GL_CCW };
			// 剔除正面还是背面
			GLenum m_cullFace{ GL_BACK };

			// 剪裁测试相关
			// 是否开启剪裁设置
			bool m_scissorSet{ false };
			// 是否开启剪裁测试
			bool m_scissorTest{ false };
			int32_t m_scissorX, m_scissorY;
			int32_t m_scissorW, m_scissorH;

			// 文字相关
			TextInfo m_textInfo;
		};
	}
}