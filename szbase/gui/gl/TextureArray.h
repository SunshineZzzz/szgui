// comment: 纹理数组

#pragma once

#ifdef USE_OPENGL_ES
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include <cstdint>
#include <string>
#include <unordered_map>

#include "CheckRstErr.h"

namespace sz_gui
{
	namespace gl
	{
        class TextureArray 
        {
        public:
            TextureArray(uint32_t unit)
            {
                m_unit = unit;
            }
            ~TextureArray()
            {
                if (m_textureArray != 0)
                {
                    GL_CALL(glDeleteTextures(1, &m_textureArray));
                }
            }

            // 创建纹理数组
            void Create(int32_t width, int32_t height, int32_t maxLayer);
            // 添加纹理
            bool AddTexture(int32_t layer, const unsigned char* bitmap, 
                int32_t bitmapWidth, int32_t bitmapHeight);
            // 纹理数组单元与纹理数组对象绑定
            void Bind() const
            {
                // 先激活纹理单元，然后绑定纹理对象
                GL_CALL(glActiveTexture(GL_TEXTURE0 + m_unit));
                GL_CALL(glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureArray));
            }
            // 获取纹理单元号
            uint32_t GetUnit() const
            {
                return m_unit;
            }
            // 获取纹理宽高
            int32_t GetWidth() const { return m_width; }
            int32_t GetHeight() const { return m_height; }
            // 获取最大纹理层数
            int32_t GetMaxLayer() const { return m_maxlayer; }

        private:
            // 纹理数组单元号
            uint32_t m_unit{ 0 };
            // 纹理数组对象
            GLuint m_textureArray{ 0 };
            // 纹理数组宽高和最大纹理层数
            int32_t m_width{ 0 }, m_height{ 0 }, m_maxlayer{ 0 };
        };
	}
}