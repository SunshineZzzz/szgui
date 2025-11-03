#include "Shader.h"
#include "CheckRstErr.h"

#include <fstream>
#include <sstream>

#include <glm/gtc/type_ptr.hpp>

namespace sz_gui
{
	namespace gl
	{
		static std::tuple<const std::string, bool> readShaderFromFile(const char* szFilePath)
		{
			std::string shaderCode;
			std::ifstream shaderFile;
			shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			try
			{
				shaderFile.open(szFilePath);
				std::stringstream shaderStream;
				shaderStream << shaderFile.rdbuf();
				shaderFile.close();
				shaderCode = shaderStream.str();
			}
			catch (std::ifstream::failure& e) 
			{
				return { e.what(), false };
			}

			return { shaderCode, true };
		}

		Shader::~Shader()
		{
			if (glIsProgram(m_program))
			{
				GL_CALL(glDeleteProgram(m_program));
				m_program = 0;
			}
		}

		std::tuple<const std::string, bool> Shader::LoadFromFile(const char* vertexShaderPath,
			const char* fragmentShaderPath)
		{
			std::string errMsg = "success";
	
			auto [vertexCode, okVS] = readShaderFromFile(vertexShaderPath);
			if (!okVS)
			{
				errMsg = "read vertex shader error," + std::string(vertexShaderPath);
				return { std::move(errMsg), false };
			}

			auto [fragmentCode, okFS] = readShaderFromFile(fragmentShaderPath);
			if (!okFS)
			{
				errMsg = "read fragment shader error," + std::string(fragmentShaderPath);
				return { std::move(errMsg), false };
			}

			return LoadFromString(vertexCode.c_str(), fragmentCode.c_str());
		}

		std::tuple<const std::string, bool> Shader::LoadFromString(const char* vertexShaderSource,
			const char* fragmentShaderSource)
		{
			std::string errMsg = "success";

			// 创建shader程序
			GLuint vertex, fragment;
			vertex = glCreateShader(GL_VERTEX_SHADER);
			fragment = glCreateShader(GL_FRAGMENT_SHADER);

			// shader程序输入代码
			glShaderSource(vertex, 1, &vertexShaderSource, NULL);
			glShaderSource(fragment, 1, &fragmentShaderSource, NULL);

			// 执行shader代码编译 
			glCompileShader(vertex);
			// 检查编译错误
			auto errStr = checkShaderErrors(vertex, "COMPILE");
			if (errStr != "")
			{
				errMsg = "compile vertex shader error," + errStr;

				glDeleteShader(vertex);
				glDeleteShader(fragment);
				return { std::move(errMsg), false };
			}
			glCompileShader(fragment);
			errStr = checkShaderErrors(fragment, "COMPILE");
			if (errStr != "")
			{
				errMsg = "compile fragment shader error," + errStr;

				glDeleteShader(vertex);
				glDeleteShader(fragment);
				return { std::move(errMsg), false };
			}

			// 创建一个Program壳子
			m_program = glCreateProgram();

			// 将vs与fs编译好的结果放到program
			glAttachShader(m_program, vertex);
			glAttachShader(m_program, fragment);

			// 执行program的链接操作，形成最终可执行shader程序
			glLinkProgram(m_program);
			// 检查链接错误
			errStr = checkShaderErrors(m_program, "LINK");
			if (errStr != "")
			{
				errMsg = "link shader error," + errStr;

				glDeleteShader(vertex);
				glDeleteShader(fragment);
				glDeleteProgram(m_program);
				m_program = 0;
				return { std::move(errMsg), false };
			}

			// 清理
			glDeleteShader(vertex);
			glDeleteShader(fragment);

			return { std::move(errMsg), true };
		}

		void Shader::Begin() const
		{
			GL_CALL(glUseProgram(m_program));
		}

		void Shader::End() const
		{
			GL_CALL(glUseProgram(0));
		}


		void Shader::SetUniformFloat(const std::string& name, float value) const
		{
			GLint location = glGetUniformLocation(m_program, name.c_str());
			GL_CALL(glUniform1f(location, value));
		}

		void Shader::SetUniformVector3(const std::string& name, float x, float y, float z) const
		{
			GLint location = glGetUniformLocation(m_program, name.c_str());
			GL_CALL(glUniform3f(location, x, y, z));
		}

		void Shader::SetUniformVector3(const std::string& name, const float* values) const
		{
			GLint location = glGetUniformLocation(m_program, name.c_str());
			GL_CALL(glUniform3fv(location, 1, values));
		}

		void Shader::SetUniformInt(const std::string& name, int value) const
		{
			GLint location = glGetUniformLocation(m_program, name.c_str());
			GL_CALL(glUniform1i(location, value));
		}

		void Shader::SetUniformMatrix4x4(const std::string& name, glm::mat4 value) const
		{
			GLint location = glGetUniformLocation(m_program, name.c_str());
			GL_CALL(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value)));
		}

		void Shader::SetUniformBool(const std::string& name, bool bValue) const
		{
			int val = bValue ? 1 : 0;
			GLint location = glGetUniformLocation(m_program, name.c_str());
			GL_CALL(glUniform1i(location, val));
		}

		const std::string Shader::checkShaderErrors(GLuint target, std::string type)
		{
			int success = 0;
			char infoLog[1024] = {};
			GLsizei logSize = sizeof(infoLog);

			if (type == "COMPILE")
			{
				glGetShaderiv(target, GL_COMPILE_STATUS, &success);
				if (!success)
				{
					glGetShaderInfoLog(target, logSize, NULL, infoLog);
					return infoLog;
				}
			}
			else if (type == "LINK")
			{
				glGetProgramiv(target, GL_LINK_STATUS, &success);
				if (!success)
				{
					glGetProgramInfoLog(target, logSize, NULL, infoLog);
					return infoLog;
				}
			}
			else
			{
				return "type is wrong";
			}

			return "";
		}
	}
}