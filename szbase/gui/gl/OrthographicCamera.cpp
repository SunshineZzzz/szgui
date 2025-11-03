#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

namespace sz_gui
{
	namespace gl
	{
		OrthographicCamera::OrthographicCamera(float l, float r, float t, float b, float n, float f)
		{
			m_left = l;
			m_right = r;
			m_top = t;
			m_bottom = b;
			m_near = n;
			m_far = f;
		}

		OrthographicCamera::~OrthographicCamera() {}

		glm::mat4 OrthographicCamera::GetProjectionMatrix()
		{
			return glm::ortho(m_left, m_right, m_bottom, m_top, m_near, m_far);
		}
	}
}