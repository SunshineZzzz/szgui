// comment: 几何体

#pragma once

#ifdef USE_OPENGL_ES
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include <cstdint>
#include <string>
#include <vector>

namespace sz_gui
{
	namespace gl
	{
		class Geometry
		{
		public:
			Geometry(size_t posCapacity, size_t colorOruvCapacity, 
				size_t indicesCapacity, bool useColor);
			~Geometry();

			// 上传
			void UploadPositions(const std::vector<float>& positions);
			void UploadColorsOrUVs(const std::vector<float>& colorsOruvs);
			void UploadIndices(const std::vector<uint32_t>& indices);
			// 上传所有
			void UploadAll(const std::vector<float>& positions,
				const std::vector<float>& uvsOrColors,
				const std::vector<uint32_t>& indices
			);
			void UploadUVs(const std::vector<float>& positions,
				const std::vector<float>& uvs,
				const std::vector<uint32_t>& indices
			);
			void UploadColors(const std::vector<float>& positions,
				const std::vector<float>& colors,
				const std::vector<uint32_t>& indices
			);
			
			// 获取VAO
			GLuint GetVao() const { return m_vao; }
			// 获取绘制索引个数
			size_t GetIndicesCount() const { return m_indicesCount; }

		private:
			// 顶点数组对象，用来存储一个Mesh网格所有的顶点属性描述信息
			GLuint m_vao{ 0 };
			// 顶点缓冲对象，用来存储顶点属性数据
			// 顶点坐标vbo
			GLuint m_posVbo{ 0 };
			// 顶点颜色vbo
			GLuint m_colorVbo{ 0 };
			// 顶点uv坐标vbo
			GLuint m_uvVbo{ 0 };
			// 元素缓冲对象/索引缓冲对象，用来存储顶点绘制顺序索引号
			GLuint m_ebo{ 0 };
			// 绘制索引个数
			size_t m_indicesCount{ 0 };
			// 是否使用颜色
			bool m_useColor{ false };
			// 顶点容量
			size_t m_posCapacity{ 0 };
			size_t m_colorOrUVCapacity{ 0 };
			size_t m_indicesCapacity{ 0 };
		};
	}
}