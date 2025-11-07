// comment: 纹理

#pragma once

#ifdef USE_OPENGL_ES
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include <cstdint>
#include <string>

#include "CheckRstErr.h"

namespace sz_gui
{
	namespace gl
	{
		class Texture
		{
		public:
			// 创建字体纹理
			static Texture* CreateFont(
				const void* pixels,
				uint32_t width,
				uint32_t height,
				uint32_t unit
			);

		public:
			Texture(uint32_t unit);
			~Texture();

			std::tuple<const std::string, bool> Load(const std::string& path);
			// 纹理单元与纹理对象绑定
			// 存在多个纹理对象绑定一个纹理单元的情况，该函数做切换纹理单元与纹理对象绑定
			void Bind() const
			{
				// 先激活纹理单元，然后绑定纹理对象
				GL_CALL(glActiveTexture(GL_TEXTURE0 + m_unit));
				GL_CALL(glBindTexture(m_textureTarget, m_texture));
			}
			// 获取纹理单元号
			unsigned int GetUnit() const
			{
				return m_unit;
			}

		private:
			// 纹理对象
			GLuint m_texture{ 0 };
			// 纹理单元号
			uint32_t m_unit{ 0 };
			// 纹理宽高
			int m_width{ 0 };
			int m_height{ 0 };
			// 纹理类型
			GLuint m_textureTarget{ GL_TEXTURE_2D };
		};
	}
}