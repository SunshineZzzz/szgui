#include "Timestamp.h"

#include <chrono>
#include <format>
#include <algorithm>
#include <cmath>
#include <ctime>

namespace sz_time 
{
	static_assert(sizeof(Timestamp) == sizeof(int64_t),
		"Timestamp should be same size as int64_t");

    using std::chrono::microseconds;
    using std::chrono::system_clock;
    using std::chrono::steady_clock;
    using std::chrono::time_point;
    using std::chrono::sys_time;
    using std::chrono::current_zone;
    using std::chrono::zoned_time;
    using std::chrono::duration_cast;

    Timestamp::Timestamp()
        : m_microSecondsSinceEpoch(0)
    {
    }

    Timestamp::Timestamp(int64_t microSecondsSinceEpochArg)
        : m_microSecondsSinceEpoch(microSecondsSinceEpochArg)
    {
    }

    void Timestamp::Swap(Timestamp& that)
    {
        std::swap(m_microSecondsSinceEpoch, that.m_microSecondsSinceEpoch);
    }

    std::string Timestamp::ToString() const
    {
        int64_t seconds = m_microSecondsSinceEpoch / kMicroSecondsPerSecond;
        int64_t microseconds_remainder = m_microSecondsSinceEpoch % kMicroSecondsPerSecond;
        return std::format("{}.{:06}", seconds, microseconds_remainder);
    }

    std::string Timestamp::ToFormattedString(bool showMicroseconds) const
    {
        sys_time<microseconds> time_point{ microseconds{m_microSecondsSinceEpoch} };
        auto local_tp = zoned_time{ current_zone(), time_point };

        if (showMicroseconds)
        {
            auto floor_sec = time_point_cast<std::chrono::seconds>(time_point);
            auto us_duration = time_point - floor_sec;
            auto microseconds_val = static_cast<int>(us_duration.count());
            return std::format("{:%Y%m%d %H:%M:%S}.{:06}", local_tp, microseconds_val);
        }
        else
        {
            return std::format("{:%Y%m%d %H:%M:%S}", local_tp);
        }
    }

    bool Timestamp::Valid() const
    {
        return m_microSecondsSinceEpoch > 0;
    }

    int64_t Timestamp::MicroSecondsSinceEpoch() const
    {
        return m_microSecondsSinceEpoch;
    }

    int64_t Timestamp::MilliSecondsSinceEpoch() const
    {
        return m_microSecondsSinceEpoch / kMicroSecondsPerMillisecond;
    }

    time_t Timestamp::SecondsSinceEpoch() const
    {
        return static_cast<time_t>(m_microSecondsSinceEpoch / kMicroSecondsPerSecond);
    }


    Timestamp Timestamp::Now()
    {
        auto now_us = duration_cast<microseconds>(
            system_clock::now().time_since_epoch()
        ).count();
        return Timestamp(now_us);
    }

    int64_t Timestamp::Ticks()
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()
        ).count();
    }

    Timestamp Timestamp::Invalid()
    {
        return Timestamp();
    }

    Timestamp Timestamp::FromUnixTime(time_t t)
    {
        return FromUnixTime(t, 0);
    }

    Timestamp Timestamp::FromUnixTime(time_t t, int microseconds)
    {
        int64_t us = static_cast<int64_t>(t) * kMicroSecondsPerSecond + microseconds;
        return Timestamp(us);
    }
}