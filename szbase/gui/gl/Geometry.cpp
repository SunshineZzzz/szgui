#include "Geometry.h"
#include "CheckRstErr.h"

#include <cassert>

namespace sz_gui
{
	namespace gl
	{
		Geometry::Geometry(size_t posSize, size_t colorOrUVSize,
			size_t indicesSize, bool useColor)
		{
			m_useColor = useColor;
			m_posCapacity = posSize * sizeof(float);
			m_colorOrUVCapacity = colorOrUVSize * sizeof(float);
			m_indicesCapacity = indicesSize * sizeof(uint32_t);

			GL_CALL(glGenBuffers(1, &m_posVbo));
			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_posVbo));
			GL_CALL(glBufferData(GL_ARRAY_BUFFER, m_posCapacity, 0, GL_DYNAMIC_DRAW));

			if (m_useColor)
			{
				GL_CALL(glGenBuffers(1, &m_colorVbo));
				GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_colorVbo));
				GL_CALL(glBufferData(GL_ARRAY_BUFFER, m_colorOrUVCapacity, 0, GL_DYNAMIC_DRAW));
			}
			else
			{
				GL_CALL(glGenBuffers(1, &m_uvVbo));
				GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_uvVbo));
				GL_CALL(glBufferData(GL_ARRAY_BUFFER, m_colorOrUVCapacity, 0, GL_DYNAMIC_DRAW));
			}

			GL_CALL(glGenBuffers(1, &m_ebo));
			GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo));
			GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indicesCapacity, 0, GL_DYNAMIC_DRAW));

			GL_CALL(glGenVertexArrays(1, &m_vao));
			GL_CALL(glBindVertexArray(m_vao));

			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_posVbo));
			GL_CALL(glEnableVertexAttribArray(0));
			GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0));

			if (m_useColor)
			{
				GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_colorVbo));
				GL_CALL(glEnableVertexAttribArray(1));
				GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0));
			}
			else
			{
				GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_uvVbo));
				GL_CALL(glEnableVertexAttribArray(1));
				GL_CALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0));
			}

			GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo));

			GL_CALL(glBindVertexArray(0));
		}
		
		Geometry::Geometry(size_t posSize, size_t uvSize,
			size_t layerSize, size_t indicesSize)
		{
			m_useColor = false;
			m_posCapacity = posSize * sizeof(float);
			m_colorOrUVCapacity = uvSize * sizeof(float);
			m_layerCapacity = layerSize * sizeof(float);
			m_indicesCapacity = indicesSize * sizeof(uint32_t);

			GL_CALL(glGenBuffers(1, &m_posVbo));
			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_posVbo));
			GL_CALL(glBufferData(GL_ARRAY_BUFFER, m_posCapacity, 0, GL_DYNAMIC_DRAW));

			GL_CALL(glGenBuffers(1, &m_uvVbo));
			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_uvVbo));
			GL_CALL(glBufferData(GL_ARRAY_BUFFER, m_colorOrUVCapacity, 0, GL_DYNAMIC_DRAW));
			
			GL_CALL(glGenBuffers(1, &m_layerVbo));
			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_layerVbo));
			GL_CALL(glBufferData(GL_ARRAY_BUFFER, m_layerCapacity, 0, GL_DYNAMIC_DRAW));

			GL_CALL(glGenBuffers(1, &m_ebo));
			GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo));
			GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indicesCapacity, 0, GL_DYNAMIC_DRAW));

			GL_CALL(glGenVertexArrays(1, &m_vao));
			GL_CALL(glBindVertexArray(m_vao));

			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_posVbo));
			GL_CALL(glEnableVertexAttribArray(0));
			GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0));

			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_uvVbo));
			GL_CALL(glEnableVertexAttribArray(1));
			GL_CALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0));
			
			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_layerVbo));
			GL_CALL(glEnableVertexAttribArray(2));
			GL_CALL(glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 1, (void*)0));

			GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo));

			GL_CALL(glBindVertexArray(0));
		}

		Geometry::~Geometry()
		{
			if (glIsVertexArray(m_vao))
			{
				GL_CALL(glDeleteVertexArrays(1, &m_vao));
				m_vao = 0;
			}

			if (glIsBuffer(m_posVbo))
			{
				GL_CALL(glDeleteBuffers(1, &m_posVbo));
				m_posVbo = 0;
			}

			if (glIsBuffer(m_colorVbo))
			{
				GL_CALL(glDeleteBuffers(1, &m_colorVbo));
				m_colorVbo = 0;
			}

			if (glIsBuffer(m_uvVbo))
			{
				GL_CALL(glDeleteBuffers(1, &m_uvVbo));
				m_uvVbo = 0;
			}

			if (glIsBuffer(m_ebo))
			{
				GL_CALL(glDeleteBuffers(1, &m_ebo));
				m_ebo = 0;
			}

			if (glIsBuffer(m_layerVbo))
			{
				GL_CALL(glDeleteBuffers(1, &m_layerVbo));
				m_layerVbo = 0;
			}
		}

		void Geometry::UploadPositions(const std::vector<float>& positions)
		{
			assert((positions.size() * sizeof(float)) <= m_posCapacity);

			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_posVbo));
			GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(float),
				positions.data()));
			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
		}

		void Geometry::UploadColorsOrUVs(const std::vector<float>& colorsOruvs)
		{
			assert((colorsOruvs.size() * sizeof(float)) <= m_colorOrUVCapacity);

			if (m_useColor)
			{
				GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_colorVbo));
				GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, colorsOruvs.size() * sizeof(float),
					colorsOruvs.data()));
				GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
				return;
			}

			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_uvVbo));
			GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, colorsOruvs.size() * sizeof(float),
				colorsOruvs.data()));
			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
		}

		void Geometry::UploadIndices(const std::vector<uint32_t>& indices)
		{
			assert((indices.size() * sizeof(uint32_t)) <= m_indicesCapacity);

			m_indicesCount = indices.size();

			GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo));
			GL_CALL(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(uint32_t),
				indices.data()));
			GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		}

		void Geometry::UploadLayers(const std::vector<float>& layers)
		{
			assert(!m_useColor);
			assert((layers.size() * sizeof(float)) <= m_layerCapacity);

			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_layerVbo));
			GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, layers.size() * sizeof(float), 
				layers.data()));
			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
		}

		void Geometry::UploadAll(const std::vector<float>& positions,
			const std::vector<float>& uvsOrColors,
			const std::vector<uint32_t>& indices
		)
		{
			if (m_useColor)
			{
				UploadColors(positions, uvsOrColors, indices);
				return;
			}
			UploadUVs(positions, uvsOrColors, indices);
		}

		void Geometry::UploadAll(const std::vector<float>& positions,
			const std::vector<float>& uvs,
			const std::vector<float>& layers,
			const std::vector<uint32_t>& indices
		)
		{
			assert(!m_useColor);
			UploadUVs(positions, uvs, indices);
			UploadLayers(layers);
		}

		void Geometry::UploadUVs(const std::vector<float>& positions,
			const std::vector<float>& uvs,
			const std::vector<uint32_t>& indices
		)
		{
			assert(!m_useColor);
			assert((positions.size() * sizeof(float)) <= m_posCapacity);
			assert((uvs.size() * sizeof(float)) <= m_colorOrUVCapacity);
			assert((indices.size() * sizeof(uint32_t)) <= m_indicesCapacity);

			m_indicesCount = indices.size();

			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_posVbo));
			GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(float),
				positions.data()));

			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_uvVbo));
			GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, uvs.size() * sizeof(float),
				uvs.data()));

			GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo));
			GL_CALL(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(uint32_t),
				indices.data()));

			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
			GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		}

		void Geometry::UploadColors(
			const std::vector<float>& positions,
			const std::vector<float>& colors,
			const std::vector<uint32_t>& indices
		)
		{
			assert(m_useColor);
			assert((positions.size() * sizeof(float)) <= m_posCapacity);
			assert((colors.size() * sizeof(float)) <= m_colorOrUVCapacity);
			assert((indices.size() * sizeof(uint32_t)) <= m_indicesCapacity);

			m_indicesCount = indices.size();

			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_posVbo));
			GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(float),
				positions.data()));

			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_colorVbo));
			GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, colors.size() * sizeof(float),
				colors.data()));

			GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo));
			GL_CALL(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(uint32_t),
				indices.data()));

			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
			GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		}
	}
}