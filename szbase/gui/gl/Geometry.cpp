#include "Geometry.h"

namespace sz_gui
{
	namespace gl
	{
		Geometry::~Geometry()
		{
			if (glIsVertexArray(m_vao))
			{
				glDeleteVertexArrays(1, &m_vao);
				m_vao = 0;
			}

			if (glIsBuffer(m_posVbo))
			{
				glDeleteBuffers(1, &m_posVbo);
				m_posVbo = 0;
			}

			if (glIsBuffer(m_uvVbo))
			{
				glDeleteBuffers(1, &m_uvVbo);
				m_uvVbo = 0;
			}

			if (glIsBuffer(m_colorVbo))
			{
				glDeleteBuffers(1, &m_colorVbo);
				m_colorVbo = 0;
			}

			if (glIsBuffer(m_ebo))
			{
				glDeleteBuffers(1, &m_ebo);
				m_ebo = 0;
			}
		}

		std::tuple<std::string, bool> Geometry::Create()
		{
			std::string errMsg = "success";

			// 创建vao
			glGenVertexArrays(1, &m_vao);
			if (m_vao == 0)
			{
				errMsg = "failed to create vao";
				return { std::move(errMsg), false };
			}
			// 绑定vao
			glBindVertexArray(m_vao);

			// 创建各种顶点缓冲区对象
			glGenBuffers(1, &m_posVbo);
			glGenBuffers(1, &m_colorVbo);
			glGenBuffers(1, &m_uvVbo);
			glGenBuffers(1, &m_ebo);
			if (m_posVbo == 0 || m_colorVbo == 0 || m_uvVbo == 0 || m_ebo == 0)
			{
				errMsg = "failed to create vbo";
				return { std::move(errMsg), false };
			}

			// 位置顶点缓冲区对象加入到顶点属性插槽
			glBindBuffer(GL_ARRAY_BUFFER, m_posVbo);
			// 位置属性放在vao的0号位置，layout(location = 0) in vec3 aPos;
			glEnableVertexAttribArray(0);
			// 描述信息加入vao
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

			// 颜色顶点缓冲区对象加入到顶点属性插槽
			glBindBuffer(GL_ARRAY_BUFFER, m_colorVbo);
			// 颜色属性放在vao的1号位置，layout(location = 1) in vec4 aColor;
			glEnableVertexAttribArray(1);
			// 描述信息加入vao
			glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*)0);
			
			// 纹理坐标顶点缓冲区对象加入到顶点属性插槽
			glBindBuffer(GL_ARRAY_BUFFER, m_uvVbo);
			// 纹理属性放在vao的2号位置，layout(location = 2) in vec2 aUV;
			glEnableVertexAttribArray(2);
			// 描述信息加入vao
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
			
			// 解绑vao
			glBindVertexArray(0);

			return { std::move(errMsg), true };
		}

		// 上传数据
		std::tuple<std::string, bool> Geometry::Upload(const std::vector<float>& positions,
			const std::vector<uint32_t>& colors, const std::vector<uint32_t>& indices)
		{
			std::string errMsg = "success";

			if (positions.empty() || colors.empty() || indices.empty())
			{
				errMsg = "empty positions or colors or indices";
				return { std::move(errMsg), false };
			}

			if (m_vao == 0 || m_posVbo == 0 ||
				m_colorVbo == 0 || m_ebo == 0)
			{
				errMsg = "vao or vbo or ebo is null";
				return { std::move(errMsg), false };
			}

			m_indicesCount = (uint32_t)indices.size();

			// 绑定vao
			glBindVertexArray(m_vao);

			// 上传position vbo
			glBindBuffer(GL_ARRAY_BUFFER, m_posVbo);
			// 使用GL_DYNAMIC_DRAW，因为UI数据每一帧都可能变化
			glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_DYNAMIC_DRAW);

			// 上传color vbo
			glBindBuffer(GL_ARRAY_BUFFER, m_colorVbo);
			glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(uint32_t), colors.data(), GL_DYNAMIC_DRAW);

			// 上传ebo
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indicesCount * sizeof(uint32_t), indices.data(), GL_DYNAMIC_DRAW);

			// 解绑vao
			glBindVertexArray(0);

			return { std::move(errMsg), true };
		}
		std::tuple<std::string, bool> Geometry::Upload(const std::vector<float>& positions,
			const std::vector<float>& uvs, const std::vector<uint32_t>& indices)
		{
			std::string errMsg = "success";

			if (positions.empty() || uvs.empty() || indices.empty())
			{
				errMsg = "empty positions or uvs or indices";
				return { std::move(errMsg), false };
			}

			if (m_vao == 0 || m_posVbo == 0 || 
				m_uvVbo == 0 || m_ebo == 0)
			{
				errMsg = "vao or vbo or ebo is null";
				return { std::move(errMsg), false };
			}

			m_indicesCount = (uint32_t)indices.size();

			// 绑定vao
			glBindVertexArray(m_vao);

			// 上传position vbo
			glBindBuffer(GL_ARRAY_BUFFER, m_posVbo);
			// 使用GL_DYNAMIC_DRAW，因为UI数据每一帧都可能变化
			glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_DYNAMIC_DRAW);

			// 上传uv vbo
			glBindBuffer(GL_ARRAY_BUFFER, m_uvVbo);
			glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float), uvs.data(), GL_DYNAMIC_DRAW);

			// 上传ebo
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indicesCount * sizeof(uint32_t), indices.data(), GL_DYNAMIC_DRAW);

			// 解绑vao
			glBindVertexArray(0);

			return { std::move(errMsg), true };
		}
	}
}