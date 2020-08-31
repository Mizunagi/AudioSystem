#include "CLog.h"
std::ofstream CLog::m_sLogStream;
uint32_t CLog::m_sLevel = ELogLevel::ASLOG_NONE;

CLog::CLog() {
}

CLog::~CLog() {
	m_sLogStream.close();
	m_sLevel = ELogLevel::ASLOG_NONE;
}

void CLog::Open(uint32_t _destlevel) {
	m_sLogStream.close();
	m_sLevel = ELogLevel::ASLOG_NONE;

	std::filesystem::create_directory("Log");
	std::ofstream file("Log/ASLog_" + CTime::GetTime_str() + ".txt", std::ios::out);
	if (!file) return;

	m_sLevel = _destlevel;
	m_sLogStream = std::move(file);
}

void CLog::Log(ELogLevel _level, std::string _log) {
	if (!m_sLogStream || !(_level & m_sLevel))return;
	std::string output{};
	output += CTime::GetTime_str() + "\nLevel:";

	switch (_level) {
	case ASLOG_TRACE:
		output += "Trace\n";
		break;
	case ASLOG_DEBUG:
		output += "Debug\n";
		break;
	case ASLOG_INFO:
		output += "Info\n";
		break;
	case ASLOG_WARN:
		output += "Warning\n";
		break;
	case ASLOG_ERROR:
		output += "Error\n";
		break;
	case ASLOG_FATAL:
		output += "Fatal\n";
		break;
	}
	output += _log + "\n\n";
	std::cout << output << std::flush;
	m_sLogStream << output << std::flush;
}

void CLog::Log(ELogLevel _level, std::string _class, std::string _method, std::string _function, std::string _comment) {
	std::stringstream strstr;
	strstr << "Class\t\t:\t" << _class <<
		"\nMethod\t\t:\t" << _method <<
		"\nFunction\t:\t" << _function;
	if (!_comment.empty())strstr << "\nComment\t\t:\t" << _comment;
	strstr << std::flush;
	CLog::Log(_level, strstr.str());
}