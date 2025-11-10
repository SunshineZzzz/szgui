// comment: OpenGL上下文

#pragma once

#ifdef USE_OPENGL_ES
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include <glm/glm.hpp>
#include <SDL3/SDL.h>
#include <stb/stb_truetype.h>

#include <tuple>
#include <string>
#include <vector>
#include <memory>
#include <stack>

#include "../IRender.h"
#include "Shader.h"
#include "Camera.h"
#include "OrthographicCamera.h"
#include "RenderItem.h"
#include "CheckRstErr.h"
#include "TextureArray.h"

namespace sz_gui 
{
    namespace gl
    {
        class GLContext : public IRender
        {
        public:
            // GL属性设置
            static std::tuple<std::string, bool> InitGLAttributes();

        public:
            GLContext(SDL_Window* window);
            ~GLContext();

            // 初始化
            std::tuple<std::string, bool> Init(int width, int height) override;
            // 构建矢量字体
            std::tuple<std::string, bool> BuildTrueType(const std::string& filename) override;
            // 绘制文字到缓冲区
            bool DrawTextToBuffer(const TextAlignment ta, const float limitWidth, const float limitHeight,
                const std::vector<int32_t>& codepoints, std::vector<float>& positions,
                std::vector<float>& uvs, std::vector<uint32_t>& indices, 
                std::vector<float>& layers) override;
            // 加入绘制数据
            void AppendDrawData(const std::vector<float>& positions,
                const std::vector<float>& colorOrUVs, const std::vector<uint32_t>& indices,
                DrawCommand cmd) override;
            // 加入文字绘制数据
            void AppendTextDrawData(const std::vector<float>& positions,
                const std::vector<float>& uvs, const std::vector<uint32_t>& indices,
                const std::vector<float>& layers, DrawCommand cmd) override;
            // 额外加入绘制指令
            void ExtraAppendDrawCommand(DrawCommand cmd) override;
            // 渲染
            void Render() override;
            // 窗口大小改变事件
            void OnWindowResize(int width, int height) override
            {
                GL_CALL(glViewport(0, 0, width, height));
                prepareCamera(width, height);
            }
            // 设置颜色主题
            void SetColorTheme(ColorTheme theme) override;

        private:
            // 上传数据到GPU
            void uploadToGPU(RenderItem* ri, const std::vector<float>& positions,
                const std::vector<float>& colorOrUVs, const std::vector<uint32_t>& indices,
                const std::vector<float>* const layers, DrawCommand cmd);
            // 获取绘制命令
            GLenum getDrawMode(DrawMode mode);
            // 绘制对象
            void renderObject(const std::unique_ptr<RenderItem>& ri);
            // 根据Material类型不同，挑选不同的shader
            std::unique_ptr<Shader>& pickShader(MaterialType type);
            // 混合相关，获取混合因子
            GLenum getBlendSFactor(BlendFuncType type);
            GLenum getBlendDFactor(BlendFuncType type);
            // 深度测试相关，获取深度测试函数
            GLenum getDepthFunc(DepthFuncType type);
            //  设置面剔除状态
            void setFaceCullingState(const std::unique_ptr<RenderItem>& ri);
            // 设置深度测试状态
            void setDepthState(const std::unique_ptr<RenderItem>& ri);
            // 设置混合状态
            void setBlendState(const std::unique_ptr<RenderItem>& ri);
            // 设置裁剪测试状态
            void setScissorState(const std::unique_ptr<RenderItem>& ri);
            // 准备摄像机
            void prepareCamera(int width, int height)
            {
                m_camera = std::make_unique<OrthographicCamera>(0.0f, float(width), 0.0f,
                    float(height), 0.0f, 1000.f);
            }
            // 根据codepoint获取数据
            std::tuple<int32_t, stbtt_packedchar*>
                getPackedCharData(int32_t codepoint);

        public:
            // 字体渲染相关静态成员
            // 纹理图集大小
            static const int ATLAS_SIZE = 4096;
            // 基准字体高度
            inline static const float FONT_HEIGHT = 32.0f;
            // ASCII字符范围
            static const int ASCII_START_CODEPOINT = 0x20;
            static const int ASCII_END_CODEPOINT = 0x7F;
            // 中文CJK字符范围
            static const int CJK_START_CODEPOINT = 0x4E00;
            static const int CJK_END_CODEPOINT = 0x9FA5;
            // CJK单次烘焙的字符块大小(估算每个4096X4096图集能容纳的安全数量)
            // 4096*4096/32*32/4(stbtt_PackSetOversampling(&spc, 2, 2)) = 4096
            // static const int CJK_BATCH_SIZE = 4096;
            static const int CJK_BATCH_SIZE = 16384;
            // 字体图集层数
            static const int FONT_LAYERS = 1 + ((CJK_END_CODEPOINT - CJK_START_CODEPOINT + 1) + CJK_BATCH_SIZE - 1) / CJK_BATCH_SIZE;

        private:
            using RenderItemLiist = std::list<std::unique_ptr<RenderItem>>;
            using RenderItemIdUnmap = std::unordered_map<uint64_t, RenderItemLiist::iterator>;

            // SDL窗口指针
            SDL_Window* m_window = nullptr;
            // gl上下文
            SDL_GLContext m_glContext = nullptr;
            // 摄像机
            std::unique_ptr<Camera> m_camera = nullptr;
            // err shader
            std::unique_ptr<Shader> m_errShader{ nullptr };
            // 颜色shader
            std::unique_ptr<Shader> m_colorShader{ nullptr };
            // 材质shader
            std::unique_ptr<Shader> m_textureShader{ nullptr };
            // 文字shader
            std::unique_ptr<Shader> m_textShader{ nullptr };
            // 不透明绘制对象
            RenderItemIdUnmap m_opacityUIUnmap;
            RenderItemIdUnmap m_opacityTextUnmap;
            RenderItemLiist m_opacityItems;
            // 透明绘制对象
            RenderItemIdUnmap m_transparentUIUnmap;
            RenderItemIdUnmap m_transparentTextUnmap;
            RenderItemLiist m_transparentItems;
            // 裁剪测试栈
            std::stack<bool> m_scissorStack;
            // 颜色主题
            ColorTheme m_colorTheme = ColorTheme::LightMode;
            // 字体烘焙结果，codepoint<->(layer, stbtt_packedchar)
            std::unordered_map<int32_t, std::pair<int32_t, stbtt_packedchar>> m_packedCharUnmap;
            // 字体纹理数组
            std::unique_ptr<TextureArray> m_fontTextureArray;
            // 字体信息
            stbtt_fontinfo m_fontInfo;
            // 基线到字体中最高字符的距离
            int32_t m_fontAscent{ 0 };
            // 基线到字体中最低字符的距离
            int32_t m_fontDescent{ 0 };
            // 行间距，表示上一行字符的descent到该行字符的ascent之间的距离
            int32_t m_fontLineGap{ 0 };
            // 字体设计单位到FONT_HEIGHT的转换比例
            float m_fontScale{ 0.0 };
        };
    }
}