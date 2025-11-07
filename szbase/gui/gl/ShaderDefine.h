// comment: shader定义

#pragma once

// 颜色顶点着色器
const char* ColorVS =
#ifdef USE_OPENGL_ES
R"(#version 300 es
precision highp float;
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 color;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
void main()
{
	vec4 transformPosition = vec4(aPos, 1.0);
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * transformPosition;
	color = aColor;
}
)";
#else
R"(#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 color;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
void main()
{
	vec4 transformPosition = vec4(aPos, 1.0);
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * transformPosition;
	color = aColor;
}
)";
#endif
// 颜色片元着色器
const char* ColorFS =
#ifdef USE_OPENGL_ES
R"(#version 300 es
precision highp float;
in vec3 color;
out vec4 FragColor;
void main()
{
	FragColor = vec4(color, 1.0);
}
)";
#else
R"(#version 460 core
in vec3 color;
out vec4 FragColor;
void main()
{
	FragColor = vec4(color, 1.0);
}
)";
#endif


// 文字顶点着色器
const char* TextVS =
#ifdef USE_OPENGL_ES
R"(#version 300 es
precision highp float;
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
out vec2 uv;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
void main()
{
	vec4 transformPosition = vec4(aPos, 1.0);
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * transformPosition;
	uv = aUV;
}
)";
#else
R"(#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
out vec2 uv;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
void main()
{
	vec4 transformPosition = vec4(aPos, 1.0);
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * transformPosition;
	uv = aUV;
}
)";
#endif
// 文字片元着色器
const char* TextFS =
#ifdef USE_OPENGL_ES
R"(#version 300 es
precision highp float;
in vec2 uv;
out vec4 FragColor;
uniform sampler2D sampler;
uniform vec3 textColor;
uniform float opacity;
void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(sampler, uv).r);
	FragColor = vec4(textColor, 1.0) * sampled * opacity;
}
)";
#else
R"(#version 460 core
in vec2 uv;
out vec4 FragColor;
uniform sampler2D sampler;
uniform vec3 textColor;
uniform float opacity;
void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(sampler, uv).r);
	FragColor = vec4(textColor, 1.0) * sampled * opacity;
}
)";
#endif