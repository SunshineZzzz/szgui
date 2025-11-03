// comment: 着色器

#pragma once

#ifdef USE_OPENGL_ES
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include <string>

#include <glm/glm.hpp>

namespace sz_gui
{
	namespace gl
	{
		class Shader 
		{
		public:
			Shader() = default;
			~Shader();

			// 从文件加载着色器
			std::tuple<const std::string, bool> LoadFromFile(const char* vertexShaderPath, 
				const char* fragmentShaderPath);
			// 从字符串加载着色器
			std::tuple<const std::string, bool> LoadFromString(const char* vertexShaderSource,
				const char* fragmentShaderSource);

			// 开始使用当前Shader
			void Begin() const;
			// 结束使用当前Shader
			void End() const;
			// 设置uniform变量
			void SetUniformFloat(const std::string& name, float value) const;
			void SetUniformVector3(const std::string& name, float x, float y, float z) const;
			void SetUniformVector3(const std::string& name, const float* values) const;
			void SetUniformInt(const std::string& name, int value) const;
			void SetUniformMatrix4x4(const std::string& name, glm::mat4 value) const;
			void SetUniformBool(const std::string& name, bool bValue) const;

		private:
			// 检测shader错误
			const std::string checkShaderErrors(GLuint target, std::string type);

			// 着色器程序Id
			GLuint m_program{ 0 };
		};
	}
}