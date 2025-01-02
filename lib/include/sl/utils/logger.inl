#pragma once

#include "sl/utils/logger.hpp"


namespace sl::utils {
	constexpr Logger::Logger(const String &name, std::ostream &stream) noexcept :
		Logger(name, DEFAULT_SEVERITY_MASK, DEFAULT_FORMAT_STRING, stream) {}
	constexpr Logger::Logger(const String &name, sl::utils::FlagField<LogSeverity> severityMask, std::ostream &stream) noexcept :
		Logger(name, severityMask, DEFAULT_FORMAT_STRING, stream) {}
	constexpr Logger::Logger(const String &name, FormatString formatString, std::ostream &stream) noexcept :
		Logger(name, DEFAULT_SEVERITY_MASK, formatString, stream) {}


	constexpr Logger::Logger(const String &name, sl::utils::FlagField<LogSeverity> severityMask, FormatString formatString, std::ostream &stream) noexcept :
		m_name {name},
		m_formatString {formatString},
		m_severityMask {severityMask},
		m_stream {&stream}
	{

	}


	template <typename ...Args>
	constexpr void Logger::log(LogSeverity severity, std::format_string<Args...> str, Args &&...args) noexcept {
		if (!(severity & m_severityMask))
			return;
		String severityString {};
		switch (severity) {
			case LogSeverity::eDebug: severityString = "debug"; break;
			case LogSeverity::eInfo:  severityString = "info"; break;
			case LogSeverity::eWarn:  severityString = "warn"; break;
			case LogSeverity::eError: severityString = "error"; break;
			default: severityString = "unknown"; break;
		}

		String severityColor {};
		switch (severity) {
			case LogSeverity::eDebug: severityColor = "\033[35m"; break;
			case LogSeverity::eInfo:  severityColor = "\033[34m"; break;
			case LogSeverity::eWarn:  severityColor = "\033[93m"; break;
			case LogSeverity::eError: severityColor = "\033[31m"; break;
		}

		*m_stream << std::format(
			m_formatString,
			std::format<Args...> (str, std::forward<Args> (args)...),
			m_name,
			severityString,
			severityColor,
			String("\033[m")
		) << std::endl;
	}

} // sl::utils
