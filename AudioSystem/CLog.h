#pragma once
#include "Utilities.h"
#include "CTime.h"

using timestamp = std::chrono::system_clock::time_point;

class CLog {
public:
	~CLog();

	enum ELogLevel {
		ASLOG_NONE = 0x0,
		ASLOG_TRACE = 0x1,
		ASLOG_DEBUG = 0x2,
		ASLOG_INFO = 0x4,
		ASLOG_WARN = 0x8,
		ASLOG_ERROR = 0x10,
		ASLOG_FATAL = 0x20,
		ASLOG_ALL = 0x3F
	};

	static void Open(uint32_t _destLevel);
	static void Log(ELogLevel _level, std::string _log);
	static void Log(ELogLevel _level, std::string _class, std::string _method, std::string _function, std::string _comment);
private:
	CLog();
	static std::ofstream m_sLogStream;
	static uint32_t m_sLevel;
};