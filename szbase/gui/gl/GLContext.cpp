#include "GLContext.h"
#include "Geometry.h"
#include "CheckRstErr.h"
#include "ShaderDefine.h"
#include "../../macro/Macro.h"

#include <unordered_set>
#include <format>
#include <algorithm>
#include <fstream>

#define STB_TRUETYPE_IMPLEMENTATION 1
#include <stb/stb_truetype.h>

namespace sz_gui
{
    namespace gl
    {
        std::tuple<std::string, bool> GLContext::InitGLAttributes()
        {
            std::string errMsg = "success";

            #ifdef USE_OPENGL_ES
            if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES) ||
                !SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3) ||
                !SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0))
            {
                errMsg = "gles version error," + std::string(SDL_GetError());
                return { std::move(errMsg), false };
            }
            #else
            if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4) ||
                !SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6) ||
                !SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE))
            {
                errMsg = "gl version error," + std::string(SDL_GetError());
                return { std::move(errMsg), false };
            }
            #endif

            // 一些基础属性
            if (!SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) ||
                !SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8) ||
                !SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8) ||
                !SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8) ||
                !SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8) ||
                !SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24) ||
                !SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8))
            {
                errMsg = "gl set base attribute error," + std::string(SDL_GetError());
                return { std::move(errMsg), false };
            }

            // 开启多重采样缓冲区 & 设置每像素的采样点数量
            //if (!SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1)||
            //    !SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4))
            //{
            //    errMsg = "gl set multisample error," + std::string(SDL_GetError());
            //    return { std::move(errMsg), false };
            //}

            return { errMsg, true };
        }

        GLContext::GLContext(SDL_Window* window)
        {
            m_window = window;
        }

        GLContext::~GLContext()
        {
            m_window = nullptr;

            if (m_glContext)
            {
                SDL_GL_DestroyContext(m_glContext);
                m_glContext = nullptr;
            }
        }

        std::tuple<std::string, bool> GLContext::Init(int width, int height)
        {
            std::string errMsg = "success";

            m_glContext = SDL_GL_CreateContext(m_window);
            if (!m_glContext)
            {
                errMsg = "sdl create gl context error," + std::string(SDL_GetError());
                return { std::move(errMsg), false };
            }

            if (!SDL_GL_MakeCurrent(m_window, m_glContext))
            {
                errMsg = "sdl make current error," + std::string(SDL_GetError());
                return { std::move(errMsg), false };
            }

            m_colorShader = std::make_unique<Shader>();
            auto [err, ok] = m_colorShader->LoadFromString(ColorVS, ColorFS);
            if (!ok)
			{
				return { err, false };
			}

            m_textShader = std::make_unique<Shader>();
            std::tie(err, ok) = m_textShader->LoadFromString(TextVS, TextFS);
            if (!ok)
			{
				return { err, false };
			}

            SetColorTheme(m_colorTheme);

            OnWindowResize(width, height);

            return { std::move(errMsg), true };
        }

        std::tuple<std::string, bool> GLContext::BuildTrueType(const std::string& filename)
        {
            std::string errMsg = "success";
            std::ifstream file(filename, std::ios::binary | std::ios::ate);
            if (!file.is_open())
            {
                errMsg = "open file error";
				return { std::move(errMsg), false };
            }

            std::streamsize file_size = file.tellg();
            file.seekg(0, std::ios::beg);

            std::vector<unsigned char> ttfBuffer(file_size);
            if (!file.read(reinterpret_cast<char*>(ttfBuffer.data()), file_size))
            {
                errMsg = "read file error";
				return { std::move(errMsg), false };
            }
            
            int result = stbtt_InitFont(&m_fontInfo, ttfBuffer.data(), 0);
            if (result == 0)
			{
				errMsg = "stbtt_InitFont failed";
				return { std::move(errMsg), false };
			}
            stbtt_GetFontVMetrics(&m_fontInfo, &m_fontAscent, &m_fontDescent, &m_fontLineGap);
            m_fontScale = stbtt_ScaleForPixelHeight(&m_fontInfo, FONT_HEIGHT);

            // 创建字体纹理数组对象
            m_fontTextureArray = std::make_unique<TextureArray>(0);
            m_fontTextureArray->Create(ATLAS_SIZE, ATLAS_SIZE, FONT_LAYERS);
            int32_t layers = 0;

            // ASCII字体烘培
            // 纹理图集内存                                                                                                                
            std::vector<unsigned char> tempBitmap;
            tempBitmap.reserve(ATLAS_SIZE * ATLAS_SIZE);
            stbtt_packedchar packedAscii[ASCII_END_CODEPOINT - ASCII_START_CODEPOINT]{};
            stbtt_pack_range range =
            { 
                FONT_HEIGHT, 
                ASCII_START_CODEPOINT, 
                NULL, 
                ASCII_END_CODEPOINT - ASCII_START_CODEPOINT,
                packedAscii, 
            };
            stbtt_pack_context spc{};
            if (!stbtt_PackBegin(&spc, tempBitmap.data(), ATLAS_SIZE, ATLAS_SIZE, 0, 1, NULL))
            {
                errMsg = "stbtt_PackBegin failed for asciii";
                return { std::move(errMsg), false };
            }
            // stbtt_PackSetOversampling(&spc, 2, 2); 
            int success = stbtt_PackFontRanges(
                &spc,
                ttfBuffer.data(),
                0,
                &range,
                1
            );
            stbtt_PackEnd(&spc); 
            if (success == 0) 
            {
                errMsg = "stbtt_PackFontRanges failed for asciii. Atlas size might be too small.";
                return { std::move(errMsg), false };
            }
            if (!m_fontTextureArray->AddTexture(layers, tempBitmap.data(), ATLAS_SIZE, ATLAS_SIZE))
            {
                errMsg = "add ascii to texture array failed";
                return { std::move(errMsg), false };
            }
            for (int i = ASCII_START_CODEPOINT; i <= ASCII_END_CODEPOINT; ++i)
            {
                m_packedCharUnmap[i] = std::make_pair(layers, packedAscii[i - ASCII_START_CODEPOINT]);
            }
            layers++;

            // CJK字体烘培
            std::vector<int32_t> codepoints;
            codepoints.reserve(CJK_END_CODEPOINT - CJK_START_CODEPOINT);
            for (int32_t cp = CJK_START_CODEPOINT; cp <= CJK_END_CODEPOINT; ++cp)
            {
                codepoints.push_back(cp);
            }
            int total = (int)codepoints.size();
            int curOffset = 0;
            uint32_t curTextureUnit = 1;
            stbtt_packedchar packedCJK[CJK_BATCH_SIZE]{};
            while (curOffset < total)
            {
                int numToPack = std::min(CJK_BATCH_SIZE, total - curOffset);
                tempBitmap.clear();
                if (!stbtt_PackBegin(&spc, tempBitmap.data(), ATLAS_SIZE, ATLAS_SIZE, 0, 1, NULL))
                {
                    return { "stbtt_PackBegin failed for CJK batch", false };
                }
                // stbtt_PackSetOversampling(&spc, 2, 2);
                stbtt_pack_range range = {
                    FONT_HEIGHT,
                    0,
                    codepoints.data() + curOffset,
                    numToPack,
                    packedCJK
                };
                success = stbtt_PackFontRanges(
                    &spc,
                    ttfBuffer.data(),
                    0,
                    &range,
                    1
                );
                stbtt_PackEnd(&spc);
                if (success == 0)
				{
					return { "stbtt_PackFontRanges failed for CJK batch. Atlas size might be too small.", false };
				}
                if (!m_fontTextureArray->AddTexture(layers, tempBitmap.data(), ATLAS_SIZE, ATLAS_SIZE))
                {
                    errMsg = "add cjk to texture array failed";
                    return { std::move(errMsg), false };
                }
                layers++;
                for (int i = 0; i < numToPack; ++i)
                {
                    m_packedCharUnmap[codepoints[curOffset + i]] = std::make_pair(curTextureUnit, packedCJK[i]);
                }

                curOffset += numToPack;
                curTextureUnit++;
            }
			return { errMsg, true };
        }

        bool GLContext::DrawTextToBuffer(const TextAlignment ta, const float limitWidth, 
            const float limitHeight, const std::vector<int32_t>& codepoints, 
            std::vector<float>& positions, std::vector<float>& uvs, std::vector<uint32_t>& indices, 
            std::vector<float>& layers)
        {
            if (codepoints.empty() || limitWidth < 0.0001f || limitHeight < 0.0001f)
            {
                return false;
            }

            positions.clear();
            uvs.clear();
            indices.clear();
            layers.clear();
            
            auto textureWidth = m_fontTextureArray->GetWidth();
            auto textureHeight = m_fontTextureArray->GetHeight();
            auto maxLayer = m_fontTextureArray->GetMaxLayer();
            int32_t layer = -1;
            stbtt_packedchar* pcData = nullptr;
            float current_x = 0.0f;
            float current_y = 0.0f;
            // 一个字符框从最高点到最低点，再加上额外行间距的总垂直高度
            float line_height = (m_fontAscent - m_fontDescent + m_fontLineGap) * m_fontScale;
            float max_w = 0.0f;
            float max_h = line_height;

            // 不缩放情况下计算绘制文本需要的总高度和总宽度
            for (int32_t codepoint : codepoints) 
            {
                std::tie(layer, pcData) = getPackedCharData(codepoint);
                if (!pcData || layer < 0 || layer > maxLayer)
				{
					return false;
				}
                
                // pcData->xadvance，渲染完这个字符后，光标应该向右移动多少距离，以便开始绘制下一个字符
                if (current_x + pcData->xadvance > limitWidth)
                {
                    max_w = std::max(max_w, current_x);
                    current_x = 0.0f;
                    max_h += line_height;
                }
                current_x += pcData->xadvance;
            }
            max_w = std::max(max_w, current_x);

            // 计算缩放比例
            float scale_x = max_w > limitWidth ? limitWidth / max_w : 1.0f;
            float scale_y = max_h > limitHeight ? limitHeight / max_h : 1.0f;
            // 缩放比例取最小值
            float scale = std::min(scale_x, scale_y);
           
            // 上面在计算缩放的时候使用的最小值，缩放以后，存在以前需要3行变成2行或者1行就够了
            max_w = 0.0f;
            max_h = line_height * scale;
            current_x = 0.0f;
            current_y = 0.0f;
            for (int32_t codepoint : codepoints)
            {
                std::tie(layer, pcData) = getPackedCharData(codepoint);
                if (current_x + pcData->xadvance * scale > limitWidth)
                {
                    max_w = std::max(max_w, current_x);
                    current_x = 0.0f;
                    max_h += line_height * scale;
                }
                current_x += pcData->xadvance * scale;
            }
            max_w = std::max(max_w, current_x);
            // 水平偏移量
            float horizontal_offset = 0.0f;
            // 垂直偏移量
            float vertical_offset = 0.0f;
            if (ta == (TextAlignment::HCenter | TextAlignment::VCenter))
            {
                if (max_w < limitWidth)
                {
                    horizontal_offset = (limitWidth - max_w) / 2.0f;
                }

                if (max_h < limitHeight)
				{
					vertical_offset = (limitHeight - max_h) / 2.0f;
				}
            }
            else
            {
                assert(0);
            }

            // 重新布局并生成顶点数据
            current_x = 0.0f;
            current_y = 0.0f;
            uint32_t vertex_offset = 0;
            for (int32_t codepoint : codepoints)
            {
                std::tie(layer, pcData) = getPackedCharData(codepoint);

                // 换行判断
                if (current_x + pcData->xadvance * scale > limitWidth)
                {
                    current_x = 0.0f;
                    current_y += line_height * scale;
                }
                
                // 计算字符的屏幕坐标
                // pcData->x0, pcData->y0, pcData->x1, pcData->y1
                // 是字符在纹理图集中的左上角和右下角坐标，需要标准化为[0,1]
                // pcData->xoff, pcData->yoff
                // https://learnopengl-cn.github.io/06%20In%20Practice/02%20Text%20Rendering/
                // xoff，bearingX从origin到字符左边缘的水平距离(通常是正数)
                // yoff，bearingY从origin到字符顶部的垂直距离(通常是负数)
                 
                // 字符左上角X坐标
                float char_x = horizontal_offset + current_x + pcData->xoff * scale;
                // 字符左上角Y坐标
                // vertical_center_offset，整体居中偏移量
                // current_y，当前行起始位置
                // (m_fontAscent * m_fontScale * scale)
                // 基线到字体中最高字符的距离
                // (pcData->yoff * scale)
                // 基线到当前字符顶部的距离，负数
                float char_y = vertical_offset + current_y
                    + (m_fontAscent * m_fontScale * scale)
                    + (pcData->yoff * scale);
                // float char_y = 0.0f;
                float char_w = (pcData->x1 - pcData->x0) * scale;
                // 字符渲染高度
                float char_h = (pcData->y1 - pcData->y0) * scale;

                // 计算纹理坐标，标准化
                float tex_width = static_cast<float>(textureWidth);
                float tex_height = static_cast<float>(textureHeight);
                float u0 = pcData->x0 / tex_width;
                float v0 = pcData->y0 / tex_height;
                float u1 = pcData->x1 / tex_width;
                float v1 = pcData->y1 / tex_height;

                // 位置数据
                positions.insert(positions.end(), 
                {
                    // 左下
                    char_x, char_y + char_h, 0.0f,
                    // 右下
                    char_x + char_w, char_y + char_h, 0.0f, 
                    // 右上
                    char_x + char_w, char_y, 0.0f,
                    // 左上
                    char_x, char_y, 0.0f              
                });

                // UV数据
                uvs.insert(uvs.end(), 
                {
                    // 左下
                    u0, v1,
                    // 右下
                    u1, v1,
                    // 右上
                    u1, v0,
                    // 左上
                    u0, v0
                });

                // 索引数据，每个字符2个三角形，6个索引
                indices.insert(indices.end(), 
                {
                     // 第一个三角形: 顺时针 (左下 -> 右上 -> 右下)
                     vertex_offset,
                     vertex_offset + 2,
                     vertex_offset + 1,

                     // 第二个三角形: 顺时针 (左下 -> 左上 -> 右上)
                     vertex_offset,
                     vertex_offset + 3,
                     vertex_offset + 2
                });

                // 纹理层数据
                layers.insert(layers.end(),
                {
                    // 左下
                    float(layer),
                    // 右下
                    float(layer),
                    // 右上
                    float(layer),
                    // 左上
                    float(layer)
                });

                current_x += pcData->xadvance * scale;
                vertex_offset += 4;
            }

            return !positions.empty();
        }

        void GLContext::AppendDrawData(const std::vector<float>& positions, 
            const std::vector<float>& colorOrUVs,
            const std::vector<uint32_t>& indices, DrawCommand cmd)
        { 
            assert(cmd.m_onlyId);
            assert(cmd.m_drawTarget == DrawTarget::UI);
            assert(cmd.m_materialType == MaterialType::ColorMaterial ||
                cmd.m_materialType == MaterialType::TextureMaterial);
            RenderItem* ri = nullptr;
            bool oldOpcacity = false;
            bool oldTransparent = false;

            auto oIt = m_opacityUIUnmap.find(cmd.m_onlyId);
            auto tIt = m_transparentUIUnmap.find(cmd.m_onlyId);
            if (oIt == m_opacityUIUnmap.end() && tIt == m_transparentUIUnmap.end())
            {
                ri = new RenderItem();
                bool useColor = (cmd.m_materialType == MaterialType::ColorMaterial);
                ri->m_geo = std::make_unique<Geometry>(
                    positions.size(),
                    colorOrUVs.size(),
                    indices.size(), 
                    useColor
                );
            }
            else if (oIt == m_opacityUIUnmap.end())
            {
                oldTransparent = true;
                ri = tIt->second->get();
            }
            else
            {
                oldOpcacity = true;
                ri = oIt->second->get();
            }

            if (sz_utils::HasFlag(cmd.m_renderState, RenderState::EnableBlend) && oldOpcacity)
            {
                auto oldRenderItem = std::move(*(oIt->second));
                m_opacityItems.erase(oIt->second);
                m_opacityUIUnmap.erase(oIt);

                auto it = m_transparentItems.insert(m_transparentItems.end(), std::move(oldRenderItem));
                m_transparentUIUnmap[cmd.m_onlyId] = it;
                oldOpcacity = false;
                oldTransparent = true;
            }

            ri->m_position = cmd.m_worldPos;
            ri->m_drawMode = getDrawMode(cmd.m_drawMode);
            ri->m_materialType = cmd.m_materialType;
            uploadToGPU(ri, positions, colorOrUVs, indices, nullptr, cmd);

            if (sz_utils::HasFlag(cmd.m_renderState, RenderState::EnableFaceCulling))
            {
                ri->m_cullFace = true;
                ri->m_frontFace = (cmd.m_faceCulling.m_frontFace ==
                    FrontFaceType::CCW ? GL_CCW : GL_CW);
                ri->m_cullFace = (cmd.m_faceCulling.m_cullFace ==
                    CullFaceType::Back ? GL_BACK : GL_FRONT);
            }
            else
            {
                ri->m_cullFace = false;
            }

            if (sz_utils::HasFlag(cmd.m_renderState, RenderState::EnableDepthTest))
            {
                ri->m_depthTest = true;
                ri->m_depthFunc = getDepthFunc(cmd.m_depthTest.m_depthFunc);
                ri->m_depthWrite = cmd.m_depthTest.m_depthWrite;
            }
            else
			{
				ri->m_depthTest = false;
                ri->m_depthWrite = false;
			}

            if (sz_utils::HasFlag(cmd.m_renderState, RenderState::EnableBlend))
            {
                ri->m_blend = true;
                ri->m_sFactor = getBlendSFactor(cmd.m_blend.m_srcBlendFunc);
                ri->m_dFactor = getBlendDFactor(cmd.m_blend.m_dstBlendFunc);
                ri->m_opacity = cmd.m_blend.m_opacity;
            }
            else
			{
				ri->m_blend = false;
			}

            if (sz_utils::HasFlag(cmd.m_renderState, RenderState::EnableScissorSet))
            {
                ri->m_scissorSet = true;
                ri->m_scissorTest = cmd.m_scissorTest.m_scissorTest;
                ri->m_scissorX = cmd.m_scissorTest.m_x;
                ri->m_scissorY = cmd.m_scissorTest.m_y;
                ri->m_scissorW = cmd.m_scissorTest.m_width;
                ri->m_scissorH = cmd.m_scissorTest.m_height;
            }
            else
            {
                ri->m_scissorSet = false;
            }

            if (oldOpcacity || oldTransparent)
            {
                return;
            }

            if (ri->m_blend)
            {
                m_transparentItems.push_back(std::unique_ptr<RenderItem>(ri));
                m_transparentUIUnmap[cmd.m_onlyId] = std::prev(m_transparentItems.end());
                return;
            }

            m_opacityItems.push_back(std::unique_ptr<RenderItem>(ri));
            m_opacityUIUnmap[cmd.m_onlyId] = std::prev(m_opacityItems.end());
        }

        void GLContext::AppendTextDrawData(const std::vector<float>& positions,
            const std::vector<float>& uvs, const std::vector<uint32_t>& indices, 
            const std::vector<float>& layers, DrawCommand cmd)
        {
            assert(cmd.m_onlyId);
            assert(cmd.m_drawTarget == DrawTarget::Text);
            assert(cmd.m_materialType == MaterialType::TextMaterial);

            RenderItem* ri = nullptr;
            bool oldOpcacity = false;
            bool oldTransparent = false;

            auto oIt = m_opacityTextUnmap.find(cmd.m_onlyId);
            auto tIt = m_transparentTextUnmap.find(cmd.m_onlyId);
            if (oIt == m_opacityTextUnmap.end() && tIt == m_transparentTextUnmap.end())
            {
                ri = new RenderItem();
                ri->m_geo = std::make_unique<Geometry>(
                    positions.size(),
                    uvs.size(), 
                    layers.size(),
                    indices.size()
                );
            }
            else if (oIt == m_opacityTextUnmap.end())
            {
                oldTransparent = true;
                ri = tIt->second->get();
            }
            else
            {
                oldOpcacity = true;
                ri = oIt->second->get();
            }

            if (sz_utils::HasFlag(cmd.m_renderState, RenderState::EnableBlend) && oldOpcacity)
            {
                auto oldRenderItem = std::move(*(oIt->second));
                m_opacityItems.erase(oIt->second);
                m_opacityTextUnmap.erase(oIt);

                auto it = m_transparentItems.insert(m_transparentItems.end(), std::move(oldRenderItem));
                m_transparentTextUnmap[cmd.m_onlyId] = it;
                oldOpcacity = false;
                oldTransparent = true;
            }

            ri->m_position = cmd.m_worldPos;
            ri->m_drawMode = getDrawMode(cmd.m_drawMode);
            ri->m_materialType = cmd.m_materialType;
            uploadToGPU(ri, positions, uvs, indices, &layers, cmd);

            if (sz_utils::HasFlag(cmd.m_renderState, RenderState::EnableFaceCulling))
            {
                ri->m_cullFace = true;
                ri->m_frontFace = (cmd.m_faceCulling.m_frontFace ==
                    FrontFaceType::CCW ? GL_CCW : GL_CW);
                ri->m_cullFace = (cmd.m_faceCulling.m_cullFace ==
                    CullFaceType::Back ? GL_BACK : GL_FRONT);
            }
            else
            {
                ri->m_faceCulling = false;
            }

            if (sz_utils::HasFlag(cmd.m_renderState, RenderState::EnableDepthTest))
            {
                ri->m_depthTest = true;
                ri->m_depthFunc = getDepthFunc(cmd.m_depthTest.m_depthFunc);
                ri->m_depthWrite = cmd.m_depthTest.m_depthWrite;
            }
            else
			{
				ri->m_depthTest = false;
                ri->m_depthWrite = false;
			}

            if (sz_utils::HasFlag(cmd.m_renderState, RenderState::EnableBlend))
            {
                ri->m_blend = true;
                ri->m_sFactor = getBlendSFactor(cmd.m_blend.m_srcBlendFunc);
                ri->m_dFactor = getBlendDFactor(cmd.m_blend.m_dstBlendFunc);
                ri->m_opacity = cmd.m_blend.m_opacity;
            }
            else
			{
				ri->m_blend = false;
			}

            if (sz_utils::HasFlag(cmd.m_renderState, RenderState::EnableScissorSet))
            {
                ri->m_scissorSet = true;
                ri->m_scissorTest = cmd.m_scissorTest.m_scissorTest;
                ri->m_scissorX = cmd.m_scissorTest.m_x;
                ri->m_scissorY = cmd.m_scissorTest.m_y;
                ri->m_scissorW = cmd.m_scissorTest.m_width;
                ri->m_scissorH = cmd.m_scissorTest.m_height;
            }
            else
			{
				ri->m_scissorSet = false;
			}

            if (oldOpcacity || oldTransparent)
            {
                return;
            }

            if (ri->m_blend)
            {
                m_transparentItems.push_back(std::unique_ptr<RenderItem>(ri));
                m_transparentTextUnmap[cmd.m_onlyId] = std::prev(m_transparentItems.end());
                return;
            }

            m_opacityItems.push_back(std::unique_ptr<RenderItem>(ri));
            m_opacityTextUnmap[cmd.m_onlyId] = std::prev(m_opacityItems.end());
        }

        void GLContext::ExtraAppendDrawCommand(DrawCommand cmd)
        {
            assert(cmd.m_onlyId);
            if (cmd.m_drawTarget != DrawTarget::UI)
            {
                return;
            }
 
            RenderItem* ri = nullptr;
            bool oldOpcacity = false;
            bool oldTransparent = false;
            auto oIt = m_opacityUIUnmap.find(cmd.m_onlyId);
            auto tIt = m_transparentUIUnmap.find(cmd.m_onlyId);
            if (oIt == m_opacityUIUnmap.end() && tIt == m_transparentUIUnmap.end())
            {
                return;
            }
            else if (oIt == m_opacityUIUnmap.end())
            {
                oldTransparent = true;
                ri = tIt->second->get();
            }
            else
            {
                oldOpcacity = true;
                ri = oIt->second->get();
            }

            if (sz_utils::HasFlag(cmd.m_renderState, RenderState::EnableScissorSet))
            {
                ri->m_scissorSet = true;
                ri->m_scissorTest = cmd.m_scissorTest.m_scissorTest;
                ri->m_scissorX = cmd.m_scissorTest.m_x;
                ri->m_scissorY = cmd.m_scissorTest.m_y;
                ri->m_scissorW = cmd.m_scissorTest.m_width;
                ri->m_scissorH = cmd.m_scissorTest.m_height;
            }
            else
            {
                ri->m_scissorSet = false;
            }
        }

        void GLContext::uploadToGPU(RenderItem* ri, const std::vector<float>& positions,
            const std::vector<float>& colorOrUVs, const std::vector<uint32_t>& indices,
            const std::vector<float>* const layers, DrawCommand cmd)
        {
            DISABLE_MSVC_WARNING(26813);
            if (cmd.m_uploadOp == UploadOperation::Retain)
            {
                return;
            }
            RESTORE_MSVC_WARNING();

            if (cmd.m_materialType == MaterialType::ColorMaterial)
            {
                if (cmd.m_uploadOp == (UploadOperation::UploadPos |
                    UploadOperation::UploadColorOrUv | UploadOperation::UploadIndex))
                {
                    ri->m_geo->UploadAll(positions, colorOrUVs, indices);
					return;
                }

                if (sz_utils::HasFlag(cmd.m_uploadOp, UploadOperation::UploadPos))
				{
					ri->m_geo->UploadPositions(positions);
				}
				if (sz_utils::HasFlag(cmd.m_uploadOp, UploadOperation::UploadColorOrUv))
				{
					ri->m_geo->UploadColorsOrUVs(colorOrUVs);
				}
				if (sz_utils::HasFlag(cmd.m_uploadOp, UploadOperation::UploadIndex))
				{
					ri->m_geo->UploadIndices(indices);
				}
                return;
            }

            if (cmd.m_materialType == MaterialType::TextureMaterial)
            {
                assert(0);
            }

            if (cmd.m_materialType == MaterialType::TextMaterial)
            {
                assert(layers != nullptr);

                if (!sz_utils::HasFlag(cmd.m_uploadOp, UploadOperation::UploadText))
                {
                    return;
                }
                ri->m_geo->UploadAll(positions, colorOrUVs, *layers, indices);
            }
        }

        void GLContext::Render()
        {
            assert(m_scissorStack.empty());

            // 设置当前帧，绘制的时候，opengl的必要状态机参数
            // 默认开启面剔除
            GL_CALL(glEnable(GL_CULL_FACE));
            GL_CALL(glFrontFace(GL_CW));
            GL_CALL(glCullFace(GL_BACK));
            
            // 默认开启深度测试
            GL_CALL(glEnable(GL_DEPTH_TEST));
            GL_CALL(glDepthFunc(GL_LEQUAL));
            GL_CALL(glDepthMask(GL_TRUE));

            // 默认关闭颜色混合
            GL_CALL(glDisable(GL_BLEND));

            // 清理画布 
            GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            // 先绘制不透明物体，透明物体按照距离摄像机远近排序，由远到近绘制
            m_transparentItems.sort(
                [this](const std::unique_ptr<RenderItem>& a, const std::unique_ptr<RenderItem>& b) {
                    auto viewMatrix = m_camera->GetViewMatrix();

                    // 计算a的相机系的Z
                    auto modelMatrixA = a->GetModelMatrix();
                    auto worldPositionA = modelMatrixA * glm::vec4(0.0, 0.0, 0.0, 1.0);
                    auto cameraPositionA = viewMatrix * worldPositionA;

                    // 计算b的相机系的Z
                    auto modelMatrixB = b->GetModelMatrix();
                    auto worldPositionB = modelMatrixB * glm::vec4(0.0, 0.0, 0.0, 1.0);
                    auto cameraPositionB = viewMatrix * worldPositionB;

                    return cameraPositionA.z < cameraPositionB.z;
                }
            );

            // 先绘制不透明物体
            for (const auto& item : m_opacityItems)
            {
                renderObject(item);
            }

            // 透明物体按照距离摄像机远近排序，由远到近绘制
            for (const auto& item : m_transparentItems)
            {
                renderObject(item);
            }

            SDL_GL_SwapWindow(m_window);
        }

        void GLContext::SetColorTheme(ColorTheme theme) 
        {
            m_colorTheme = theme;

            switch (theme)
            {
            case ColorTheme::LightMode:
                GL_CALL(glClearColor(0.97f, 0.97f, 0.97f, 1.0f));
                break;
            }
        }

        GLenum GLContext::getDrawMode(DrawMode mode)
        {
            switch (mode)
            {
            case DrawMode::LINE_LOOP:
                return GL_LINE_LOOP;
            case DrawMode::TRIANGLES:
                return GL_TRIANGLES;
            default:
                assert(0);
            }
            return GL_TRIANGLES;
        }

        void GLContext::renderObject(const std::unique_ptr<RenderItem>& ri)
        {
            // 设置渲染状态
            setFaceCullingState(ri);
            setDepthState(ri);
            setBlendState(ri);
            
            // 决定使用哪个Shader 
            auto& shader = pickShader(ri->m_materialType);
            shader->Begin();

            switch (ri->m_materialType)
            {
            case MaterialType::ColorMaterial:
                // mvp
                shader->SetUniformMatrix4x4("modelMatrix", ri->GetModelMatrix());
                shader->SetUniformMatrix4x4("viewMatrix", m_camera->GetViewMatrix());
                shader->SetUniformMatrix4x4("projectionMatrix", m_camera->GetProjectionMatrix());
				break;
			case MaterialType::TextureMaterial:
                assert(0);
				break;
			case MaterialType::TextMaterial:
                // mvp
                shader->SetUniformMatrix4x4("modelMatrix", ri->GetModelMatrix());
                shader->SetUniformMatrix4x4("viewMatrix", m_camera->GetViewMatrix());
                shader->SetUniformMatrix4x4("projectionMatrix", m_camera->GetProjectionMatrix());
                // 字体纹理数组
                shader->SetUniformInt("sampler", (int)m_fontTextureArray->GetUnit());
                m_fontTextureArray->Bind();
                // 透明度
                shader->SetUniformFloat("opacity", ri->m_opacity);
                // 文字颜色
                shader->SetUniformVector3("textColor", ri->m_textInfo.m_color);
				break;
			default:
                assert(0);
            }

            // 绑定vao
            GL_CALL(glBindVertexArray(ri->m_geo->GetVao()));
            // 绘制
            GL_CALL(glDrawElements(ri->m_drawMode, (GLsizei)ri->m_geo->GetIndicesCount(), GL_UNSIGNED_INT, 0));

            // 设置剪裁状态
            setScissorState(ri);
        }

        std::unique_ptr<Shader>& GLContext::pickShader(MaterialType type)
        {
            switch (type)
            {
            case MaterialType::ColorMaterial:
                return m_colorShader;
			case MaterialType::TextMaterial:
				return m_textShader;
			default:
				assert(0);
            }

            return m_errShader;
        }

        GLenum GLContext::getBlendSFactor(BlendFuncType type)
        {
            switch (type)
            {
            case BlendFuncType::SRC_ALPHA:
                return GL_SRC_ALPHA;
            default:
                return GL_SRC_ALPHA;
            }
        }

        GLenum GLContext::getBlendDFactor(BlendFuncType type)
        {
            switch (type)
            {
            case BlendFuncType::ONE_MINUS_SRC_ALPHA:
                return GL_ONE_MINUS_SRC_ALPHA;
            default:
                return GL_ONE_MINUS_SRC_ALPHA;
            }
        }

        GLenum GLContext::getDepthFunc(DepthFuncType type)
        {
            switch (type)
			{
            case DepthFuncType::Less:
				return GL_LESS;
            case DepthFuncType::Lequal:
                return GL_LEQUAL;
            case DepthFuncType::Greater:
                return GL_GREATER;
			default:
				return GL_GREATER;
			}
        }
        
        void GLContext::setFaceCullingState(const std::unique_ptr<RenderItem>& ri)
        {
            if (ri->m_faceCulling)
            {
                GL_CALL(glEnable(GL_CULL_FACE));
                GL_CALL(glFrontFace(ri->m_frontFace));
                GL_CALL(glCullFace(ri->m_cullFace));
            }
            else
            {
                GL_CALL(glDisable(GL_CULL_FACE));
            }
        }

        void GLContext::setDepthState(const std::unique_ptr<RenderItem>& ri)
        {
            if (ri->m_depthTest)
            {
                GL_CALL(glEnable(GL_DEPTH_TEST));
                GL_CALL(glDepthFunc(ri->m_depthFunc));
            }
            else
            {
                GL_CALL(glDisable(GL_DEPTH_TEST));
            }

            if (ri->m_depthWrite)
            {
                GL_CALL(glDepthMask(GL_TRUE));
            }
            else 
            {
                GL_CALL(glDepthMask(GL_FALSE));
            }
        }

        void GLContext::setBlendState(const std::unique_ptr<RenderItem>& ri)
        {
            if (ri->m_blend)
            {
                GL_CALL(glEnable(GL_BLEND));
                GL_CALL(glBlendFunc(ri->m_sFactor, ri->m_dFactor));
            }
            else
            {
                GL_CALL(glDisable(GL_BLEND));
            }
        }

        void GLContext::setScissorState(const std::unique_ptr<RenderItem>& ri)
        {
            if (!ri->m_scissorSet)
            {
                return;
            }

            if (ri->m_scissorTest)
			{
                m_scissorStack.push(true);

				GL_CALL(glEnable(GL_SCISSOR_TEST));
                GL_CALL(glScissor(ri->m_scissorX, ri->m_scissorY, ri->m_scissorW, ri->m_scissorH));
			}
			else
			{
                m_scissorStack.pop();

				GL_CALL(glDisable(GL_SCISSOR_TEST));
			}
        }

        std::tuple<int32_t, stbtt_packedchar*>
            GLContext::getPackedCharData(int32_t codepoint)
        {
            const auto& it = m_packedCharUnmap.find(codepoint);
            if (it != m_packedCharUnmap.end())
            {
                return { it->second.first, &it->second.second};
            }
            return { -1, nullptr};
        }
    }
}