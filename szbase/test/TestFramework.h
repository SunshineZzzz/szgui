#pragma once

#include <iostream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <functional>

namespace sz_test
{
    // ������־����
    enum class LogLevel
    {
        OK,
        FAIL,
        INFO,
        SECTION
    };

    // ȫ����־�����������
    using LogFunction = std::function<void(LogLevel level, const std::string& message)>;

    // Ĭ�ϵ���־ʵ��
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

    // ȫ����־����ָ�룬Ĭ��Ϊ default_log_func
    inline LogFunction g_logger = default_log_func;

    // �ⲿ�ӿڣ������Զ�����־����
    inline void set_logger(LogFunction logger)
    {
        g_logger = std::move(logger);
    }

    // �ڲ����ýӿ�
    inline void log(LogLevel level, const std::string& message)
    {
        g_logger(level, message);
    }


    // TEST_ASSERT: ��������Ƿ�Ϊ�棬ʧ�������������Ϣ���˳�
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

        // TEST_EQUAL: �������ֵ�Ƿ���ȣ�ʧ�������������Ϣ���˳�
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