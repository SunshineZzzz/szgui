// comment: 几何体

#pragma once

#include <GLES3/gl3.h>

#include <cstdint>
#include <tuple>
#include <string>
#include <vector>

#include "../../ds/Math.h"

namespace sz_gui
{
	namespace gles
	{
		class Geometry
		{
		public:
			Geometry() = default;
			~Geometry();

			// 创建VAO, VBO, EBO并配置顶点属性
			std::tuple<std::string, bool> Create();
			// 上传数据
			std::tuple<std::string, bool> Upload(const std::vector<sz_ds::Vertex>& vertices, 
				const std::vector<uint32_t>& indices);

			// 获取VAO
			GLuint GetVao() const { return m_vao; }
			// 获取绘制索引个数
			uint32_t GetIndicesCount() const { return m_indicesCount; }

		private:
			// 顶点数组对象，用来存储一个Mesh网格所有的顶点属性描述信息
			GLuint m_vao{ 0 };
			// 顶点缓冲对象，用来存储顶点属性数据
			// 顶点坐标vbo
			GLuint m_posVbo{ 0 };
			// 颜色坐标vbo
			GLuint m_colorVbo{ 0 };
			// 顶点uv坐标vbo
			GLuint m_uvVbo{ 0 };
			// 元素缓冲对象/索引缓冲对象，用来存储顶点绘制顺序索引号
			GLuint m_ebo{ 0 };
			// 绘制索引个数
			uint32_t m_indicesCount{ 0 };
		};
	}
}