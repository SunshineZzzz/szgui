#include "Texture2d.h"

#define STB_IMAGE_IMPLEMENTATION 1
#include <stb/stb_image.h>

#include <format>

namespace sz_gui
{
	namespace gles
	{
		Texture2D::Texture2D(uint32_t unit):
			m_unit(unit)
		{}

		Texture2D::~Texture2D()
		{
			if (glIsTexture(m_texture))
			{
				glDeleteTextures(1, &m_texture);
				m_texture = 0;
			}
		}

		std::tuple<const std::string, bool> Texture2D::Load(const std::string& path)
		{
			std::string errMsg = "success";

			int channels;
			// 反转y轴
			stbi_set_flip_vertically_on_load(true);
			unsigned char* data = stbi_load(path.c_str(), &m_width, &m_height, &channels, STBI_rgb_alpha);
			if (!data)
			{
				errMsg = std::format("stbi_load error,{},{}", stbi_failure_reason(), path);
				return { std::move(errMsg), false };
			}
			(void)channels;

			// 创建纹理对象
			glGenTextures(1, &m_texture);
			// 激活纹理单元
			glActiveTexture(GL_TEXTURE0 + m_unit);
			// 绑定纹理对象
			glBindTexture(GL_TEXTURE_2D, m_texture);
			// 纹理对象m_texture就被对应到了纹理单元GL_TEXTURE0+m_unit
			// 开辟显存，并上传数据
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

			// 自动生成mipmap
			glGenerateMipmap(GL_TEXTURE_2D);

			// 释放数据
			stbi_image_free(data);

			// 设置纹理的过滤方式
			// 采样(Sampling)：动作。是从纹理或场景中读取一个或多个点的数据的过程。
			// 过滤(Filtering)：方法/处理。是根据采样的点和预设的算法（如平均、插值）来计算出一个最终颜色的过程。
			// 
			// GL_TEXTURE_MAG_FILTER (放大过滤)
			// 发生时机：屏幕像素 > 纹理像素 (纹理被拉伸)
			// 描述：屏幕上需要的像素比实际纹理对象像素多，采用线性过滤
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// GL_TEXTURE_MIN_FILTER (缩小过滤)
			// 发生时机：屏幕像素 < 纹理像素 (纹理被压缩)
			// 描述：屏幕上需要的像素比实际纹理对象像素少，采用临近过滤
			// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			// GL_LINEAR_MIPMAP_LINEAR: 在单个mipmap上采用线性采样，在两层mipmap LOD之间(比如L=1.3,L1,L2之间)采用线性过滤来获取纹理像素
			// GL_NEAREST_MIPMAP_NEAREST: 在单个mipmap上采用临近采样，在两层mipmap LOD之间(比如L=1.3,L1,L2之间)采用临近过滤来获取纹理像素
			// 还有其他的组合方式，比如GL_LINEAR_MIPMAP_NEAREST，GL_NEAREST_MIPMAP_LINEAR等
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

			// 设置纹理的包裹方式
			// 纹理坐标超出[0,1]范围，采用重复模式
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			// v纹理坐标超出[0,1]范围，采用重复模式
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			return { std::move(errMsg), true };
		}
	}
}