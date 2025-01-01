#pragma once

#include <iostream>

#include "sl/core.hpp"
#include "sl/utils/enums.hpp"
#include "sl/utils/string.hpp"


namespace sl::utils {
	enum class LogSeverity : sl::utils::FlagEnumBit {
		eDebug = 0_feb,
		eInfo  = 1_feb,
		eWarn  = 2_feb,
		eError = 3_feb
	};

	class SL_CORE Logger {
		using String = sl::utils::BasicString<char, std::allocator<char>>;
		public:
			/* 0 : log's content
			 * 1 : logger's name
			 * 2 : log's severity
			 * 3 : log's color start
			 * 4 : log's color end
			 * */
			using FormatString = std::format_string<std::string, const String&, String&, String&, String>;
			static constexpr FormatString DEFAULT_FORMAT_STRING {"{3}[{1}] {2} > {0}{4}"};
			static constexpr sl::utils::FlagField<LogSeverity> DEFAULT_SEVERITY_MASK {LogSeverity::eDebug | LogSeverity::eInfo | LogSeverity::eWarn | LogSeverity::eError};

			constexpr Logger(const String &name, std::ostream &stream = std::cout) noexcept;
			constexpr Logger(const String &name, sl::utils::FlagField<LogSeverity> severityMask, std::ostream &stream = std::cout) noexcept;
			constexpr Logger(const String &name, FormatString formatString, std::ostream &stream = std::cout) noexcept;
			constexpr Logger(const String &name, sl::utils::FlagField<LogSeverity> severityMask, FormatString formatString, std::ostream &stream = std::cout) noexcept;

			template <typename ...Args>
			constexpr void log(LogSeverity severity, std::format_string<Args...> str, Args &&...args) noexcept;
			template <typename ...Args>
			constexpr void debug(std::format_string<Args...> str, Args &&...args) noexcept {return this->log(LogSeverity::eDebug, str, args...);}
			template <typename ...Args>
			constexpr void info(std::format_string<Args...> str, Args &&...args) noexcept {return this->log(LogSeverity::eInfo, str, args...);}
			template <typename ...Args>
			constexpr void warn(std::format_string<Args...> str, Args &&...args) noexcept {return this->log(LogSeverity::eWarn, str, args...);}
			template <typename ...Args>
			constexpr void error(std::format_string<Args...> str, Args &&...args) noexcept {return this->log(LogSeverity::eError, str, args...);}

			constexpr void setSeverityMask(sl::utils::FlagField<LogSeverity> severityMask) noexcept {m_severityMask = severityMask;}
			constexpr void setStream(std::ostream &stream) noexcept {m_stream = &stream;}


		private:
			const String m_name;
			FormatString m_formatString;
			sl::utils::FlagField<LogSeverity> m_severityMask;
			std::ostream *m_stream;
	};

	extern Logger mainLogger;

} // namespace sl::utils

#include "sl/utils/logger.inl"
