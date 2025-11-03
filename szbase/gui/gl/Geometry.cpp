#include "Geometry.h"
#include "CheckRstErr.h"

namespace sz_gui
{
	namespace gl
	{
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
		}

		void Geometry::UploadUVs(const std::vector<float>& positions,
			const std::vector<float>& uvs,
			const std::vector<uint32_t>& indices
		)
		{
			m_indicesCount = indices.size();

			GL_CALL(glGenBuffers(1, &m_posVbo));
			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_posVbo));
			GL_CALL(glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), 
				positions.data(), GL_STATIC_DRAW));

			GL_CALL(glGenBuffers(1, &m_uvVbo));
			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_uvVbo));
			GL_CALL(glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float), 
				uvs.data(), GL_STATIC_DRAW));

			GL_CALL(glGenBuffers(1, &m_ebo));
			GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo));
			GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
				indices.data(), GL_STATIC_DRAW));

			GL_CALL(glGenVertexArrays(1, &m_vao));
			GL_CALL(glBindVertexArray(m_vao));

			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_posVbo));
			GL_CALL(glEnableVertexAttribArray(0));
			GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0));

			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_uvVbo));
			GL_CALL(glEnableVertexAttribArray(1));
			GL_CALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0));

			GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo));

			GL_CALL(glBindVertexArray(0));
		}

		void Geometry::UploadColors(
			const std::vector<float>& positions,
			const std::vector<float>& colors,
			const std::vector<uint32_t>& indices
		)
		{
			m_indicesCount = indices.size();

			GL_CALL(glGenBuffers(1, &m_posVbo));
			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_posVbo));
			GL_CALL(glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), 
				positions.data(), GL_STATIC_DRAW));

			GL_CALL(glGenBuffers(1, &m_colorVbo));
			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_colorVbo));
			GL_CALL(glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), 
				colors.data(), GL_STATIC_DRAW));

			GL_CALL(glGenBuffers(1, &m_ebo));
			GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo));
			GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
				indices.data(), GL_STATIC_DRAW));

			GL_CALL(glGenVertexArrays(1, &m_vao));
			GL_CALL(glBindVertexArray(m_vao));

			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_posVbo));
			GL_CALL(glEnableVertexAttribArray(0));
			GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0));

			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_colorVbo));
			GL_CALL(glEnableVertexAttribArray(1));
			GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0));

			GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo));

			GL_CALL(glBindVertexArray(0));
		}
	}
}