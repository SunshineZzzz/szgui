// comment: 数学相关

#pragma once

#include <glm/glm.hpp>

#include <limits>

namespace sz_ds
{
    // 常用类型别名
    template<typename T>
    class AxisAlignedBox2D;
    using AABB2D = AxisAlignedBox2D<float>;
    using AABB2Dd = AxisAlignedBox2D<double>;
    template<typename T>
    class VertexData;
    using Vertex = VertexData<float>;
    using Vertexd = VertexData<double>;

    // 4字节颜色
    struct Rgba4Byte
    {
        Rgba4Byte(unsigned char r = 255, unsigned char g = 255,
            unsigned char b = 255, unsigned char a = 255)
        {
            m_r = r;
            m_g = g;
            m_b = b;
            m_a = a;
        }

        // 将RGBA四分量打包成一个 32 位无符号整数
        std::uint32_t ToUint32() const
        {
            return (static_cast<std::uint32_t>(m_r) << 24) |
                (static_cast<std::uint32_t>(m_g) << 16) |
                (static_cast<std::uint32_t>(m_b) << 8) |
                (static_cast<std::uint32_t>(m_a));
        }

        // 转换为int
        operator int() const
        {
            return static_cast<int>(static_cast<unsigned>(*this));
        }

    public:
        unsigned char m_r;
        unsigned char m_g;
        unsigned char m_b;
        unsigned char m_a;
    };

    // color是否相等
    inline bool operator==(const Rgba4Byte& left, const Rgba4Byte& right)
    {
        return (left.m_r == right.m_r &&
            left.m_g == right.m_g &&
            left.m_b == right.m_b &&
            left.m_a == right.m_a);
    }

    // 顶点数据
    template<typename T>
    class VertexData
    {
    public:
        using Vec3 = glm::vec<3, T, glm::defaultp>;
        using Vec2 = glm::vec<2, T, glm::defaultp>;

        // 坐标
        Vec3 m_position;
        // 纹理
        Vec3 m_texture;
        // 颜色
        Rgba4Byte m_color;
    };

    // 2D轴对齐边界框(Axis-Aligned Bounding Box, AABB)
    template<typename T>
    class AxisAlignedBox2D
    {
    public:
        using Vec2 = glm::vec<2, T, glm::defaultp>;

        // AABB的有效范围状态
        enum class Extent
        {
            // 盒子尚未初始化
            ENULL,
            // 盒子正常
            EFINITE,
            // 盒子在某些或所有维度上是无限大的
            EINFINITE
        };

    public:
        // 矩形边界框的最小角点
        Vec2 m_minimum;
        // 矩形边界框的最大角点
        Vec2 m_maximum;
        // 矩形边界框的范围状态
        Extent m_extent;

    public:
        AxisAlignedBox2D()
        {
            setNull();
        }
        ~AxisAlignedBox2D() = default;

        AxisAlignedBox2D(const AxisAlignedBox2D& other)
        {
            setExtents(other.m_minimum, other.m_maximum);
            m_extent = other.m_extent;
        }

        AxisAlignedBox2D(const Vec2& min, const Vec2& max)
        {
            setExtents(min, max);
            m_extent = Extent::EFINITE;
        }

        AxisAlignedBox2D(T mx, T my, T Mx, T My)
        {
            setExtents(mx, my, Mx, My);
            m_extent = Extent::EFINITE;
        }

        AxisAlignedBox2D<T>& operator=(const AxisAlignedBox2D<T>& other)
        {
            if (this == &other)
            {
                return *this;
            }

            setExtents(other.m_minimum, other.m_maximum);
            m_extent = other.m_extent;
            return *this;
        }

        // 获取最小角点
        const Vec2& GetMinimum() const
        {
            return m_minimum;
        }
        // 设置最小角点
        void SetMinimum(const Vec2& vec)
        {
            m_minimum = vec;
        }
        void SetMinimum(T x, T y)
        {
            m_minimum = Vec2(x, y);
        }
        // 获取最大角点
        const Vec2& GetMaximum() const
        {
            return m_maximum;
        }
        // 设置最大角点
        void SetMaximum(const Vec2& vec)
        {
            m_maximum = vec;
        }
        void SetMaximum(T x, T y)
        {
            m_maximum = Vec2(x, y);
        }
        // 检查当前AABB是否与另一个AABB相交
        bool IsIntersects(const AxisAlignedBox2D& other) const
        {
            if (m_maximum.x < other.m_minimum.x)
            {
                return false;
            }
            if (m_maximum.y < other.m_minimum.y)
            {
                return false;
            }
            if (m_minimum.x > other.m_maximum.x)
            {
                return false;
            }
            if (m_minimum.y > other.m_maximum.y)
            {
                return false;
            }
            return true;
        }
        // 检查AABB是否包含指定的点
        bool Contains(const Vec2& v) const
        {
            return (m_minimum.x <= v.x && v.x <= m_maximum.x &&
                m_minimum.y <= v.y && v.y <= m_maximum.y);
        }
        // 检查当前AABB是否完全包含另一个AABB
        bool Contains(const AxisAlignedBox2D& other) const
        {
            return (m_minimum.x <= other.m_minimum.x && 
                m_minimum.y <= other.m_minimum.y &&
                other.m_maximum.x <= m_maximum.x &&
                other.m_maximum.y <= m_maximum.y);
        }
        // 计算当前AABB与另一个AABB的交集，返回一个新的AABB
        AxisAlignedBox2D<T> Intersection(const AxisAlignedBox2D<T>& other) const
        {
            Vec2 intMin = m_minimum;
            Vec2 intMax = m_maximum;

            intMin = glm::max(intMin, other.GetMinimum());
            intMax = glm::min(intMax, other.GetMaximum());

            if (intMin.x < intMax.x &&
                intMin.y < intMax.y)
            {
                return AxisAlignedBox2D<T>(intMin, intMax);
            }

            return AxisAlignedBox2D<T>();
        }
        // 扩展AABB以包含指定的点
        void Merge(Vec2 point)
        {
            if (m_minimum.x > point.x)
            {
                m_minimum.x = point.x;
            }

            if (m_minimum.y > point.y)
            {
                m_minimum.y = point.y;
            }

            if (m_maximum.x < point.x)
            {
                m_maximum.x = point.x;
            }

            if (m_maximum.y < point.y)
            {
                m_maximum.y = point.y;
            }

            m_extent = Extent::EFINITE;
        }
        // 扩展AABB以包含另一个AABB
        void Merge(AxisAlignedBox2D<T> other)
        {
            m_maximum = glm::max(m_maximum, other.GetMaximum());
            m_minimum = glm::min(m_minimum, other.GetMinimum());
            m_extent = Extent::EFINITE;
        }
        // 获取AABB的中心点
        Vec2 GetCenter(void) const
        {
            return Vec2(
                (m_maximum.x + m_minimum.x) * T(0.5f),
                (m_maximum.y + m_minimum.y) * T(0.5f)
            );
        }
        // 获取AABB的尺寸
        Vec2 GetSize() const
        {
            return m_maximum - m_minimum;
        }
        // 获取AABB的半尺寸
        Vec2 GetHalfSize() const
        {
            return (m_maximum - m_minimum) * T(0.5);
        }
        // 检查AABB是否处于空状态
        bool IsNull() const
        {
            return (m_extent == Extent::ENULL);
        }
        // 检查AABB是否是有限状态
        bool IsFinite(void) const
        {
            return (m_extent == Extent::EFINITE);
        }
        // 检查AABB是否是无限状态
        bool IsInfinite(void) const
        {
            return (m_extent == Extent::EINFINITE);
        }
        // 判断两个AABB是否相等
        bool operator==(const AxisAlignedBox2D& other) const
        {
            return (m_minimum == other.m_minimum && 
                m_maximum == other.m_maximum &&
                m_extent == other.m_extent);
        }
        // 判断两个AABB是否不相等
        bool operator!=(const AxisAlignedBox2D& other) const
        {
            return !(*this == other);
        }

    private:
        // 同时设置最小和最大边界，并将状态设为有限
        void setExtents(const Vec2& min, const Vec2& max)
        {
            m_minimum = min;
            m_maximum = max;
            m_extent = Extent::EFINITE;
        }
        // 通过四个标量设置边界，并将状态设为有限
        void setExtents(T mx, T my, T Mx, T My)
        {
            m_minimum.x = mx;
            m_minimum.y = my;
            m_maximum.x = Mx;
            m_maximum.y = My;
            m_extent = Extent::EFINITE;
        }
        // 将AABB重置为空状态
        void setNull()
        {
            T max_val = std::numeric_limits<T>::max();
            T lowest_val = std::numeric_limits<T>::lowest();

            m_minimum = Vec2(max_val);
            m_maximum = Vec2(lowest_val);
            m_extent = Extent::ENULL;
        }
    };
}