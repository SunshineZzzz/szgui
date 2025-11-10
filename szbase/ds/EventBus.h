// comment: 事件总线

#pragma once

#include <iostream>
#include <unordered_map>
#include <typeindex>
#include <utility>
#include <cassert>

#include "Delegate.h"

namespace sz_ds
{
    // 事件抽象
    class IEvent
    {
    public:
        virtual ~IEvent() = default;
        // 类型索引
        virtual std::type_index GetType() const = 0;
    };

    // 事件
    template<typename TEventType>
    class Event : public IEvent
    {
    public:
        using Type = TEventType;

    public:
        explicit Event(Type data) : m_data(std::move(data)) {}

        // 获取事件类型
        std::type_index GetType() const override 
        { 
            return std::type_index(typeid(Type)); 
        }

        // 获取事件数据
        const Type& GetData() const 
        { 
            return m_data; 
        }

    private:
        // 事件数据
        Type m_data;
    };

    // 事件总线
    class EventBus
    {
    private:
        using EventHandler = sz_ds::Delegate<void, const IEvent&>;
        using HandlerMap = std::unordered_map<uint64_t, EventHandler>;
        using EventMap = std::unordered_map<std::type_index, HandlerMap>;

    public:
        // 不用全局也可以独立使用
        EventBus() = default;
        ~EventBus() = default;

        // 获取单例实例
        static EventBus& GetInstance() 
        {
            static EventBus instance;
            return instance;
        }

        // 订阅事件
        template<typename EventType>
        uint64_t Subscribe(EventHandler handler) 
        {
            // 生成唯一的订阅ID
            uint64_t subscriptionId = next_subscription_id++;
            if (subscriptionId == 0) [[unlikely]]
			{
                assert(0);
			}

            // 将处理器插入到对应事件类型的handlerMap中
            auto& handlerMap = m_eventMap[std::type_index(typeid(typename EventType::Type))];

            // 插入处理器
            handlerMap.insert({ subscriptionId, std::move(handler) });

            return subscriptionId;
        }

        template<typename EventType, typename HandlerFunc>
        uint64_t Subscribe(HandlerFunc handler)
        {
            // 生成唯一的订阅ID
            uint64_t subscriptionId = next_subscription_id++;
            if (subscriptionId == 0) [[unlikely]]
            {
                assert(0);
            }

            EventHandler eventDelegate;
            eventDelegate.Bind(std::move(handler));

            auto& handlerMap = m_eventMap[std::type_index(typeid(typename EventType::Type))];
            handlerMap.insert({ subscriptionId, std::move(eventDelegate) });

            return subscriptionId;
        }

        // 取消订阅
        template<typename EventType>
        void Unsubscribe(uint64_t subscriptionId) 
        {
            auto it = m_eventMap.find(std::type_index(typeid(typename EventType::Type)));
            if (it != m_eventMap.end()) 
            {
                auto& handlerMap = it->second;

                handlerMap.erase(subscriptionId);

                // 如果没有订阅者了，移除该事件类型
                // if (handlerMap.empty()) 
                // {
                //     m_eventMap.erase(it);
                // }
            }
        }

        // 发布事件
        template<typename EventType, typename... Args>
        void Publish(Args&&... args) 
        {
            using EventDataType = typename EventType::Type;
            EventType event(EventDataType(std::forward<Args>(args)...));

            auto it = m_eventMap.find(std::type_index(typeid(EventDataType)));
            if (it == m_eventMap.end()) 
            {
                return;
            }

            for (const auto& pair : it->second) 
            {
                pair.second(event);
            }
        }

        // 清空所有事件订阅
        void Clear() 
        {
            m_eventMap.clear();
        }

    private:
        // 禁止复制/移动
        EventBus(const EventBus&) = delete;
        EventBus& operator=(const EventBus&) = delete;
        EventBus(EventBus&&) = delete;
        EventBus& operator=(EventBus&&) = delete;

        // 事件map，用于存储事件类型和对应的处理器
        EventMap m_eventMap;
        // 订阅ID生成器
        uint64_t next_subscription_id = 1;
    };

    // 辅助函数
    template<typename EventType>
    uint64_t SubscribeEvent(sz_ds::Delegate<void, const IEvent&> handler) 
    {
        return EventBus::GetInstance().Subscribe<EventType>(handler);
    }
    template<typename EventType>
    void UnsubscribeEvent(uint64_t subscriptionId) 
    {
        EventBus::GetInstance().Unsubscribe<EventType>(subscriptionId);
    }
    template<typename EventType, typename... Args>
    void PublishEvent(Args&&... args) 
    {
        EventBus::GetInstance().Publish<EventType>(std::forward<Args>(args)...);
    }
    inline void clearAllEvents()
    {
        EventBus::GetInstance().Clear();
    }
}