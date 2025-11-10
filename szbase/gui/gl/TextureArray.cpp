#include "TextureArray.h"

namespace sz_gui
{
	namespace gl
	{
        void TextureArray::Create(int32_t width, int32_t height, int32_t maxLayers)
        {
            m_width = width;
            m_height = height;
            m_maxlayer = maxLayers;

            GL_CALL(glGenTextures(1, &m_textureArray));
            GL_CALL(glActiveTexture(GL_TEXTURE0 + m_unit));
            GL_CALL(glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureArray));

            GL_CALL(glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RED,
                m_width, m_height, m_maxlayer, 0,
                GL_RED, GL_UNSIGNED_BYTE, nullptr));

            GL_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

            GL_CALL(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
        }

        bool TextureArray::AddTexture(int32_t layer, const unsigned char* bitmap, 
            int32_t bitmapWidth, int32_t bitmapHeight)
        {
            if (layer >= m_maxlayer ||
                bitmapWidth != m_width || 
                bitmapHeight != m_height)
            {
                return false;
            }

            GL_CALL(glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureArray));

            GL_CALL(glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
                0, 0, layer,
                m_width, m_height, 1,
                GL_RED, GL_UNSIGNED_BYTE, bitmap));

            GL_CALL(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
            
            return true;
        }
	}
}