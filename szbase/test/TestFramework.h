#pragma once

#include <iostream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <functional>

namespace sz_test
{
    // 定义日志级别
    enum class LogLevel
    {
        OK,
        FAIL,
        INFO,
        SECTION
    };

    // 全局日志输出函数类型
    using LogFunction = std::function<void(LogLevel level, const std::string& message)>;

    // 默认的日志实现
    inline void default_log_func(LogLevel level, const std::string& message)
    {
        switch (level)
        {
        case LogLevel::OK:
            std::cout << "[ OK ] " << message << std::endl;
            break;
        case LogLevel::FAIL:
            std::cerr << "[FAIL] " << message << std::endl;
            break;
        case LogLevel::INFO:
            std::cout << ">>> " << message << std::endl;
            break;
        case LogLevel::SECTION:
            std::cout << "\n===================================================" << std::endl;
            std::cout << "--- " << message << " ---" << std::endl;
            std::cout << "===================================================" << std::endl;
            break;
        }
    }

    // 全局日志函数指针，默认为 default_log_func
    inline LogFunction g_logger = default_log_func;

    // 外部接口：设置自定义日志函数
    inline void set_logger(LogFunction logger)
    {
        g_logger = std::move(logger);
    }

    // 内部调用接口
    inline void log(LogLevel level, const std::string& message)
    {
        g_logger(level, message);
    }


    // TEST_ASSERT: 检查条件是否为真，失败则输出错误信息并退出
#define TEST_ASSERT(condition, message) \
        do { \
            if (!(condition)) { \
                std::ostringstream oss; \
                oss << "Line " << __LINE__ << " in " << __FILE__ << ": " \
                    << message << " (Condition: " << #condition << ")"; \
                log(LogLevel::FAIL, oss.str()); \
                std::exit(1); \
            } else { \
                log(LogLevel::OK, message); \
            } \
        } while (0)

        // TEST_EQUAL: 检查两个值是否相等，失败则输出错误信息并退出
#define TEST_EQUAL(expected, actual, message) \
        do { \
            if ((expected) != (actual)) { \
                std::ostringstream oss; \
                oss << "Line " << __LINE__ << " in " << __FILE__ << ": " \
                    << message << " (Expected: " << (expected) << ", Actual: " << (actual) << ")"; \
                log(LogLevel::FAIL, oss.str()); \
                std::exit(1); \
            } else { \
                log(LogLevel::OK, message); \
            } \
        } while (0)

    inline void print_section(const std::string& title)
    {
        log(LogLevel::SECTION, title);
    }

    inline void print_subsection(const std::string& title)
    {
        log(LogLevel::INFO, title);
    }
}