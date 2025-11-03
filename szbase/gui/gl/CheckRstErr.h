#pragma once 

#ifdef USE_OPENGL_ES
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include <format>
#include <iostream>

#ifdef _DEBUG
	#define GL_STRINGIFY(x) #x
	#define GL_CALL(func) func;checkRstErr(GL_STRINGIFY(func), __FILE__, __LINE__);
#else
	#define GL_CALL(func) func;
#endif

static void checkRstErr(const char* szFuncName, const char* szFileName, int iLine)
{
	GLenum errorCode = glGetError();
	if (errorCode != GL_NO_ERROR) 
	{
		const std::string& errorMessage = std::format("exec opengl function error,{},{},{},{}", 
			errorCode, szFuncName, szFileName, iLine);
		std::cerr << errorMessage << std::endl;
	}
}