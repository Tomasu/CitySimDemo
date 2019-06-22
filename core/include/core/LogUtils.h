//
// Created by Thomas on 18/10/17.
//

#ifndef VOIP_CLIENT_LOGUTILS_H
#define VOIP_CLIENT_LOGUTILS_H

#include <cerrno>
#include <cstring>
#include <cstdio>
#include <utility>
#include <type_traits>

#define xstr_(a) #a
#define xstr(a) xstr_(a)

#define LOG_FILE_PREFIX "citysim"
#define PREFIX_FORMAT "%s:%d:%s: "
#define LOG_PRINT_PREFIX_ "%s:%d: "

const char *trimFunctionName(const char *func);
const char *trimFilePrefix(const char *path, const char *prefix);

#define LOG_LINE_INFO_DEF_(TAG, FILE, LINE, FUNC, VAR) LogLineInfo VAR (TAG, FILE, LINE, FUNC)
#define LOG_LINE_INFO_DEF(VAR) LOG_LINE_INFO_DEF_(TAG, __FILE__, __LINE__, __PRETTY_FUNCTION__, VAR)
#define LOG_LINE_INFO_(TAG) LogLineInfo(TAG, __FILE__, __LINE__, __PRETTY_FUNCTION__)
#define LOG_LINE_INFO() LOG_LINE_INFO_(TAG)

enum LogLevel {
	NONE = 0,
	TRACE,
	DEBUG,
	WARN,
	INFO,
	ERROR,
	FATAL,
	_MAX_LEVEL
};

const char *LogLevelStr(int level);

struct LogLineInfo {
  const char *tag;
  const char *file;
  const int line;
  const char *func;

  LogLineInfo(const char *tag_, const char *file_, const int line_, const char *func_) :
      tag(tag_), file(trimFilePrefix(file_, LOG_FILE_PREFIX)), line(line_), func(trimFunctionName(func_)) { }
};

#include <string>
template< typename T >
std::string AnyToString(const T &d);

//template< typename T >
//std::string AnyToString(const T &d);

//template< typename T >
//std::string AnyToString(T d);

//template< typename T >
inline const char *StdStrToCStr(const std::string &str)
{
	return str.c_str();
}

void log_print_(int severity, const LogLineInfo &info__, const char *format, ...);

template <typename ...ARGS>
void tmpl_printf(int severity, const LogLineInfo &info__, const char *format, ARGS&&...args)
{
	log_print_(severity, info__, format, StdStrToCStr(args)...);
}

template <typename ...ARGS>
void log_print_tmpl_(int level, const LogLineInfo &info__, const char *format, ARGS&&...args)
{
	tmpl_printf(level, info__, format, AnyToString(args)...);
}


//  __attribute__ ((format(printf, 3, 4))); --- not using so we don't trip -Wformat-security errors

#define log_print_info_(INFO, FORMAT, ...) log_print_tmpl_(LogLevel::INFO, INFO, FORMAT, ##__VA_ARGS__)
#define log_print_warn_(INFO, FORMAT, ...) log_print_tmpl_(LogLevel::WARN, INFO, FORMAT, ##__VA_ARGS__)
#define log_print_error_(INFO, FORMAT, ...) log_print_tmpl_(LogLevel::ERROR, INFO, FORMAT, ##__VA_ARGS__)
#define log_print_fatal_(INFO, FORMAT, ...) log_print_tmpl_(LogLevel::FATAL, INFO, FORMAT, ##__VA_ARGS__)
#define log_print_debug_(INFO, FORMAT, ...) log_print_tmpl_(LogLevel::DEBUG, INFO, FORMAT, ##__VA_ARGS__)
#define log_print_trace_(INFO, FORMAT, ...) log_print_tmpl_(LogLevel::TRACE, INFO, FORMAT, ##__VA_ARGS__)

#define log_info_(tag, file, line, func, ...) ({ LOG_LINE_INFO_DEF_(tag, file, line, func, info__); log_print_info_(info__, ##__VA_ARGS__); })
#define log_warn_(tag, file, line, func, ...) ({ LOG_LINE_INFO_DEF_(tag, file, line, func, info__); log_print_warn_(info__, ##__VA_ARGS__); })
#define log_error_(tag, file, line, func, ...) ({ LOG_LINE_INFO_DEF_(tag, file, line, func, info__); log_print_error_(info__, ##__VA_ARGS__); })
#define log_fatal_(tag, file, line, func, ...) ({ LOG_LINE_INFO_DEF_(tag, file, line, func, info__); log_print_fatal_(info__, ##__VA_ARGS__); })
#define log_debug_(tag, file, line, func, ...) ({ LOG_LINE_INFO_DEF_(tag, file, line, func, info__); log_print_debug_(info__, ##__VA_ARGS__); })
#define log_trace_(tag, file, line, func, ...) ({ LOG_LINE_INFO_DEF_(tag, file, line, func, info__); log_print_trace_(info__, ##__VA_ARGS__); })

#define log_info(...) ({ LOG_LINE_INFO_DEF(info__); log_print_info_(info__, ##__VA_ARGS__); })
#define log_warn(...) ({ LOG_LINE_INFO_DEF(info__); log_print_warn_(info__, ##__VA_ARGS__); })
#define log_error(...) ({ LOG_LINE_INFO_DEF(info__); log_print_error_(info__, ##__VA_ARGS__); })
#define log_fatal(...) ({ LOG_LINE_INFO_DEF(info__); log_print_fatal_(info__, ##__VA_ARGS__); })
#define log_debug(...) ({ LOG_LINE_INFO_DEF(info__); log_print_debug_(info__, ##__VA_ARGS__); })
#define log_trace_enter() ({ LOG_LINE_INFO_DEF(info__); log_print_trace_(info__, "enter"); })
#define log_trace_exit() ({ LOG_LINE_INFO_DEF(info__); log_print_trace_(info__, "exit"); })

#define formatString(OUT, OUTLEN, FORMAT, ...) formatString_(LOG_LINE_INFO(), OUT, OUTLEN, FORMAT, ##__VA_ARGS__)

template<typename T, typename... ARGS>
T formatString_(const LogLineInfo &info__, T out, int outLen,
                const char *format, ARGS&&... args) {

  //log_print_debug_(LOG_LINE_INFO_("LogUtils"), "enter");
  if(out == nullptr) {
    log_print_error_(info__, "Null out buffer");
    return nullptr;
  }

  // we want to ignore -Wformat-security for this one call otherwise
  // we get an annoying warning we don't actually care about

//#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Wformat-security"
  int ret = snprintf(out, (size_t) outLen, format, std::forward<ARGS>(args)...);
//#pragma clang diagnostic pop

  if(ret < 0) {
    log_print_debug_(LOG_LINE_INFO_("LogUtils"), "error formating string");
    return nullptr;
  }

  return out;
}

class QVector3D;
std::ostream & operator << (std::ostream &out, const QVector3D &v);

#include <vector>
#include <ostream>
template<typename T>
std::ostream & operator << (std::ostream &out, const std::vector<T> &v)
{
	//out << "QVector{";
	bool haveitem = false;
	for (auto &item: v)
	{
		if (haveitem)
		{
			out << ", ";
		}

		out << item;
		haveitem = true;
	}

	//out << "}";
	return out;
}


#endif //VOIP_CLIENT_LOGUTILS_H


