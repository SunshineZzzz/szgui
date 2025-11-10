// comment: 委托，不会主动在堆上分配内存，性能敏感，可以使用

#pragma once

#include <assert.h>
#include <new>
#include <type_traits>

namespace sz_ds
{
    static constexpr size_t MAX_DELEGATE_SIZE = 48;
    static constexpr size_t MAX_ALIGN = alignof(std::max_align_t);

    // 委托接口
    template<typename R, typename... Args>
    class IDelegate
    {
    public:
        virtual ~IDelegate() = default;
        virtual R invoke(Args... args) = 0;
        virtual void move_construct(void* buf) noexcept = 0;
        virtual void copy_construct(void* buf) const = 0;
        virtual bool is_bound() const noexcept = 0;
    };

    // 空委托
    template<typename R, typename... Args>
    class EmptyDelegate final : public  IDelegate<R, Args...>
    {
    public:
        EmptyDelegate() = default;

        void move_construct(void* buf) noexcept override 
        {
            new(buf) EmptyDelegate;
        }

        void copy_construct(void* buf) const override 
        {
            new(buf) EmptyDelegate;
        }

        R invoke(Args...) override 
        {
            if constexpr (std::is_same_v<R, void>) {}
            else { return R{}; }
        }

        bool is_bound() const noexcept override { return false; }
    };

    // 成员函数委托实现
    template<typename T, typename R, typename... Args>
    class DelegateImp : public IDelegate<R, Args...>
    {
    public:
        using Method = R(T::*)(Args...);

        DelegateImp(T* object, Method method)
            : m_object(object), m_method(method)
        {
        }

        void move_construct(void* buf) noexcept override
        {
            new(buf) DelegateImp(std::exchange(m_object, nullptr), m_method);
        }

        void copy_construct(void* buf) const override
        {
            new(buf) DelegateImp(m_object, m_method);
        }

        R invoke(Args... args) override
        {
            return (m_object->*m_method)(std::forward<Args>(args)...);
        }

        bool is_bound() const noexcept override { return true; }

    private:
        T* m_object;
        Method m_method;
    };

    // 静态/全局函数委托实现
    template<typename R, typename... Args>
    class DelegateImpStatic : public IDelegate<R, Args...>
    {
    public:
        using Method = R(*)(Args...);

        explicit DelegateImpStatic(Method method)
            : m_method(method)
        {
        }

        void move_construct(void* buf) noexcept override
        {
            new(buf) DelegateImpStatic(std::exchange(m_method, nullptr));
        }

        void copy_construct(void* buf) const override
        {
            new(buf) DelegateImpStatic(m_method);
        }

        R invoke(Args... args) override
        {
            return (*m_method)(std::forward<Args>(args)...);
        }

        bool is_bound() const noexcept override { return true; }

    private:
        Method m_method;
    };

    // Lambda/std::function/Functor委托
    template<typename F, typename R, typename... Args>
    class DelegateImpFunctor final : public IDelegate<R, Args...>
    {
    public:
        template<typename FunctorType>
        explicit DelegateImpFunctor(FunctorType&& functor)
            : m_functor(std::forward<FunctorType>(functor))
        {
        }

        void move_construct(void* buf) noexcept override
        {
            new(buf) DelegateImpFunctor(std::move(m_functor));
        }

        void copy_construct(void* buf) const override
        {
            new(buf) DelegateImpFunctor(m_functor);
        }

        R invoke(Args... args) override
        {
            return m_functor(std::forward<Args>(args)...);
        }

        bool is_bound() const noexcept override { return true; }

    private:
        F m_functor;
    };

    // 委托
    template<typename R, typename... Args>
    class Delegate
    {
    private:
        using IDelegateBase = IDelegate<R, Args...>;

        void empty()
        {
            new(static_cast<void*>(m_realObject)) EmptyDelegate<R, Args...>;
            m_delegate = static_cast<IDelegateBase*>(static_cast<void*>(m_realObject));
        }

    public:
        Delegate() noexcept
        {
            empty();
        }

        ~Delegate() noexcept
        {
            if (m_delegate)
            {
                m_delegate->~IDelegate();
                m_delegate = nullptr;
            }
        }

        // 拷贝构造
        Delegate(const Delegate& rhs) noexcept 
        {
            if (rhs.m_delegate) 
            {
                rhs.m_delegate->copy_construct(m_realObject);
            }
            else 
            {
                new(static_cast<void*>(m_realObject)) EmptyDelegate<R, Args...>;
            }
            m_delegate = static_cast<IDelegateBase*>(static_cast<void*>(m_realObject));
        }

        // 移动构造
        Delegate(Delegate&& rhs) noexcept
        {
            if (rhs.m_delegate)
            {
                rhs.m_delegate->move_construct(m_realObject);
                m_delegate = static_cast<IDelegateBase*>(static_cast<void*>(m_realObject));

                rhs.empty();
            }
            else
            {
                empty();
            }
        }

        // 拷贝赋值
        Delegate& operator=(const Delegate& rhs) noexcept
        {
            if (this == &rhs)
            {
                return *this;
            }

            if (m_delegate)
            {
                m_delegate->~IDelegate();
                m_delegate = nullptr;
            }

            if (rhs.m_delegate)
            {
                rhs.m_delegate->copy_construct(m_realObject);
                m_delegate = static_cast<IDelegateBase*>(static_cast<void*>(m_realObject));
            }
            else
            {
                empty();
            }
            return *this;
        }

        // 移动赋值
        Delegate& operator=(Delegate&& rhs) noexcept
        {
            if (this == &rhs) 
            {
                return *this;
            }

            if (m_delegate) 
            {
                m_delegate->~IDelegate();
                m_delegate = nullptr;
            }

            if (rhs.m_delegate) 
            {
                rhs.m_delegate->move_construct(m_realObject);
                m_delegate = static_cast<IDelegateBase*>(static_cast<void*>(m_realObject));
                rhs.empty();
            }
            else 
            {
                empty();
            }
            return *this;
        }

        // 绑定成员函数
        template<typename T>
        void Bind(T* object, R(T::* method)(Args...))
        {
            static_assert(sizeof(DelegateImp<T, R, Args...>) <= MAX_DELEGATE_SIZE,
                "DelegateImp is too large for the internal buffer.");
            static_assert(alignof(DelegateImp<T, R, Args...>) <= MAX_ALIGN,
                "DelegateImp alignment is too large for the internal buffer.");
            if (m_delegate)
            {
                m_delegate->~IDelegate();
                m_delegate = nullptr;
            }

            if (object && method)
            {
                new(m_realObject) DelegateImp<T, R, Args...>(object, method);
                m_delegate = static_cast<IDelegateBase*>(static_cast<void*>(m_realObject));
            }
            else
            {
                empty();
            }
        }

        // 绑定静态/全局函数
        void Bind(R(*method)(Args...))
        {
            static_assert(sizeof(DelegateImpStatic<R, Args...>) <= MAX_DELEGATE_SIZE,
                "DelegateImpStatic is too large for the internal buffer.");
            static_assert(alignof(DelegateImpStatic<R, Args...>) <= MAX_ALIGN,
                "DelegateImpStatic alignment is too large for the internal buffer.");
            
            if (m_delegate)
            {
                m_delegate->~IDelegate();
                m_delegate = nullptr;
            }

            if (method)
            {
                new(m_realObject) DelegateImpStatic<R, Args...>(method);
                m_delegate = static_cast<IDelegateBase*>(static_cast<void*>(m_realObject));
            }
            else
            {
                empty();
            }
        }

        // 绑定任意可调用对象(Functor, std::function, 捕获/非捕获Lambda)
        template<typename F>
        void Bind(F&& functor)
        {
            // 获取Functor的真实类型（去除引用和cv限定符）
            using FunctorType = std::decay_t<F>;
            using ImpType = DelegateImpFunctor<FunctorType, R, Args...>;

            // 编译时检查大小和对齐
            static_assert(sizeof(ImpType) <= MAX_DELEGATE_SIZE,
                "DelegateImpFunctor is too large for the internal buffer.");
            static_assert(alignof(ImpType) <= MAX_ALIGN,
                "DelegateImpFunctor alignment is too large for the internal buffer.");

            // 销毁当前绑定的对象
            if (m_delegate)
            {
                m_delegate->~IDelegate();
                m_delegate = nullptr;
            }

            // 检查FunctorType是否可调用且参数匹配
            if constexpr (std::is_invocable_v<FunctorType, Args...>)
            {
                // 在内部缓冲区上构造新的DelegateImpFunctor实例
                new(m_realObject) ImpType(std::forward<F>(functor));
                m_delegate = static_cast<IDelegateBase*>(static_cast<void*>(m_realObject));
            }
            else
            {
                // 如果不可调用，设置为空委托
                empty();
            }
        }

        // 调用
        R operator()(Args... args) const
        {
            if (m_delegate)
            {
                return m_delegate->invoke(std::forward<Args>(args)...);
            }
            if constexpr (std::is_same_v<R, void>) {}
            else
            {
                return R{};
            }
        }

        // bool判断，是否有绑定函数
        explicit operator bool() const noexcept
        {
            return m_delegate && m_delegate->is_bound();
        }

    private:
        alignas(MAX_ALIGN) char m_realObject[MAX_DELEGATE_SIZE];
        IDelegateBase* m_delegate;
    };
}