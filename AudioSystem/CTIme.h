#pragma once
#pragma once
#include"Utilities.h"

using timestamp = std::chrono::system_clock::time_point;
using millisecond = std::chrono::milliseconds;
using nanosecond = std::chrono::nanoseconds;

class CTime {
public:
	CTime() {};
	~CTime() {};

	static timestamp GetTime() {
		return std::chrono::system_clock::now();
	}

	static std::string GetTime_str() {
		auto now = std::chrono::system_clock::now();
		auto now_c = std::chrono::system_clock::to_time_t(now);
		std::stringstream ss;
		tm localTime;
		localtime_s(&localTime, &now_c);
		ss << std::put_time(&localTime, "%Y%m%d_%H%M%S");
		return ss.str();
	}

	template<typename T>
	static uint64_t GetDuration(timestamp _start, timestamp _end) {
		return std::chrono::duration_cast<T>(_end - _start).count();
	}
};