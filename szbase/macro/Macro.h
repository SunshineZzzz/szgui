#pragma once

// 禁用并保存警告状态
#define DISABLE_MSVC_WARNING(warning_number) \
    __pragma(warning(push)) \
    __pragma(warning(disable: warning_number))
// 恢复警告到之前保存的状态
#define RESTORE_MSVC_WARNING() \
    __pragma(warning(pop))