// comment: 时间戳实现

#pragma once

#include <cstdint>
#include <string>

namespace sz_time 
{
	// 时间戳
	class Timestamp
	{
	public:
		Timestamp();
		explicit Timestamp(int64_t microSecondsSinceEpochArg);

		// 交换函数
		void Swap(Timestamp& that);
		// 返回 秒.xxxxxx(微妙)
		std::string ToString() const;
		// 转换成一个格式化字符串.[xxxxxx(微妙)]
		std::string ToFormattedString(bool showMicroseconds = true) const;
		// 大于0有效
		bool Valid() const;
		// 返回从1970年到当前的时间，单位微秒
		int64_t MicroSecondsSinceEpoch() const;
		// 返回从1970年到当前的时间，单位毫秒
		int64_t MilliSecondsSinceEpoch() const;
		// 从1970年到当前的秒
		time_t SecondsSinceEpoch() const;
		// 获取当前的时间戳
		static Timestamp Now();
		// 获取当前tick
		static int64_t Ticks();
		// 获取一个无效时间，即时间等于0
		static Timestamp Invalid();
		// 从unixtime来生成
		static Timestamp FromUnixTime(time_t t);
		// 从unixtime来生成
		static Timestamp FromUnixTime(time_t t, int microseconds);
		// 每秒等于多少微秒
		static const int kMicroSecondsPerSecond = 1000 * 1000;
		// 每秒等于多少毫秒
		static const int kMilliSecondsPerSecond = 1000;
		// 每毫秒等于多少微秒
		static const int kMicroSecondsPerMillisecond = 1000;

	private:
		// 从1970年到当前的时间，单位微秒
		int64_t m_microSecondsSinceEpoch;
	};
	// 小于
	inline bool operator<(Timestamp lhs, Timestamp rhs)
	{
		return lhs.MicroSecondsSinceEpoch() < rhs.MicroSecondsSinceEpoch();
	}
	// 判等
	inline bool operator==(Timestamp lhs, Timestamp rhs)
	{
		return lhs.MicroSecondsSinceEpoch() == rhs.MicroSecondsSinceEpoch();
	}
	// 实现两个时间的差S
	inline double TimeDifference(Timestamp high, Timestamp low)
	{
		int64_t diff = high.MicroSecondsSinceEpoch() - low.MicroSecondsSinceEpoch();
		return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
	}
	// 实现两个时间的差MS
	inline int TimeDifferenceMs(Timestamp high, Timestamp low)
	{
		int64_t diff = high.MicroSecondsSinceEpoch() - low.MicroSecondsSinceEpoch();
		return static_cast<int>(diff / Timestamp::kMicroSecondsPerMillisecond);
	}
	// 时间戳基础上加上指定的秒
	inline Timestamp AddTime(Timestamp timestamp, double seconds)
	{
		int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
		return Timestamp(timestamp.MicroSecondsSinceEpoch() + delta);
	}
}