// comment: 2D纹理

#pragma once

#include <cstdint>
#include <string>

#include <GLES3/gl3.h>

namespace sz_gui
{
	namespace gles
	{
		class Texture2D
		{
		public:
			Texture2D(uint32_t unit);
			~Texture2D();

			std::tuple<const std::string, bool> Load(const std::string& path);
			// 纹理单元与纹理对象绑定
			// 存在多个纹理对象绑定一个纹理单元的情况，该函数做切换纹理单元与纹理对象绑定
			void Bind() const
			{
				// 先激活纹理单元，然后绑定纹理对象
				glActiveTexture(GL_TEXTURE0 + m_unit);
				glBindTexture(GL_TEXTURE_2D, m_texture);
			}

		private:
			// 纹理对象
			GLuint m_texture{ 0 };
			// 纹理单元号
			uint32_t m_unit{ 0 };
			// 纹理宽高
			int m_width{ 0 };
			int m_height{ 0 };
		};
	}
}