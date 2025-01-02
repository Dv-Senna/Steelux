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
			static constexpr FormatString DEFAULT_FORMAT_STRING {"{3}[{1}] {2} >{4} {0}"};
			static constexpr FormatString DEFAULT_FORMAT_STRING_COLORLESS {"[{1}] {2} > {0}"};
			static constexpr sl::utils::FlagField<LogSeverity> DEFAULT_SEVERITY_MASK {LogSeverity::eDebug | LogSeverity::eInfo | LogSeverity::eWarn | LogSeverity::eError};

			constexpr Logger(const String &name, std::ostream &stream = std::cout) noexcept;
			constexpr Logger(const String &name, sl::utils::FlagField<LogSeverity> severityMask, std::ostream &stream = std::cout) noexcept;
			constexpr Logger(const String &name, FormatString formatString, std::ostream &stream = std::cout) noexcept;
			constexpr Logger(const String &name, sl::utils::FlagField<LogSeverity> severityMask, FormatString formatString, std::ostream &stream = std::cout) noexcept;

			template <typename ...Args>
			constexpr auto log(LogSeverity severity, std::format_string<Args...> str, Args &&...args) noexcept -> void;
			template <typename ...Args>
			constexpr auto debug(std::format_string<Args...> str, Args &&...args) noexcept -> void {return this->log<Args...> (LogSeverity::eDebug, str, std::forward<Args> (args)...);}
			template <typename ...Args>
			constexpr auto info(std::format_string<Args...> str, Args &&...args) noexcept -> void {return this->log<Args...> (LogSeverity::eInfo, str, std::forward<Args> (args)...);}
			template <typename ...Args>
			constexpr auto warn(std::format_string<Args...> str, Args &&...args) noexcept -> void {return this->log<Args...> (LogSeverity::eWarn, str, std::forward<Args> (args)...);}
			template <typename ...Args>
			constexpr auto error(std::format_string<Args...> str, Args &&...args) noexcept -> void {return this->log<Args...> (LogSeverity::eError, str, std::forward<Args> (args)...);}

			constexpr auto setSeverityMask(sl::utils::FlagField<LogSeverity> severityMask) noexcept -> void {m_severityMask = severityMask;}
			constexpr auto setStream(std::ostream &stream) noexcept -> void {m_stream = &stream;}
			constexpr auto setFormatString(FormatString formatString) noexcept -> void {m_formatString = formatString;}


		private:
			const String m_name;
			FormatString m_formatString;
			sl::utils::FlagField<LogSeverity> m_severityMask;
			std::ostream *m_stream;
	};


} // namespace sl::utils

namespace sl {
	SL_CORE extern sl::utils::Logger mainLogger;
} // namespace sl

#include "sl/utils/logger.inl"
