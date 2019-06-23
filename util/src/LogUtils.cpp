//
// Created by Thomas on 22/11/17.
//


#include <cstring>
#include <cerrno>
#include <cstdio>
#include <cstdarg>
#include <utility>
#include <sys/prctl.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <algorithm>
#include <pthread.h>

#include "util/LogUtils.h"

#define TAG "LogUtils"

static char *yankFunctionName(char *dest, int destLength, const char *src, int srcLength);

#define THREAD_NAMEID_FMT_PREFIX "nthr"

static void getThreadName(char *dest, int len)
{
	char threadName[32] = { 0 };

	prctl(PR_GET_NAME, &threadName, 0, 0, 0);

	int nameLen = strlen(threadName);

	if (nameLen < 1)
	{
		strcpy(threadName, THREAD_NAMEID_FMT_PREFIX);
		nameLen = strlen(threadName);
	}

	int availSpace = sizeof(threadName) - nameLen;

	auto tid = (pid_t) syscall(SYS_gettid);
	if (tid != 0)
	{
		snprintf(threadName+nameLen, availSpace, "-%04d", tid);
	}
// 	else
// 	{
// 		snprintf(threadName+nameLen, availSpace, THREAD_NAMEID_FMT_PREFIX "-????");
// 	}

	strncpy(dest, threadName, len);
}

void log_print_(int severity, const LogLineInfo &info__, const char *format, ...)
{
	va_list ap;

	//char threadName[18] = { 0 };
	char threadName[32] = { 0 };
	memset(threadName, 0, sizeof(threadName));

// 	if (prctl(PR_GET_NAME, &threadName, 0, 0, 0) != 0)
// 	{
// 		//tid = pthread_gettid_np(pthread_self());
//
// 		auto tid = (pid_t) syscall(SYS_gettid);
// 		if (tid == 0)
// 		{
// 			snprintf(threadName, sizeof(threadName), THREAD_NAMEID_FMT_PREFIX "%04d", tid);
// 		}
// 		else
// 		{
// 			snprintf(threadName, sizeof(threadName), THREAD_NAMEID_FMT_PREFIX "-????");
// 		}
// 	}

	getThreadName(threadName, sizeof(threadName));


	char buffer[1024] = { 0 }; // temporary fixed length buffer. logs will get cut off if they are longer
	va_start(ap, format);
	int length = vsnprintf(nullptr, 0, format, ap);
	va_end(ap);

	if (length <= 0)
	{
		//__android_log_print(ANDROID_LOG_WARN, "LogUtils", "error building log string");
		//return;
	}

	va_start(ap, format);
	vsnprintf(buffer, sizeof(buffer), format, ap);
	va_end(ap);

	char funcNameBuffer[84] = { 0 }; // if the function name is longer than this, its ok to truncate.
	// makes logs easier to read
	yankFunctionName(funcNameBuffer, sizeof(funcNameBuffer), info__.func, strlen(info__.func));

	printf("%s [%s] %s: " LOG_PRINT_PREFIX_ "%s\n",
			LogLevelStr(severity),
			threadName,
			info__.tag,
			funcNameBuffer,
			info__.line,
			buffer);
}

static const char *LogLevelStrings[] = {
	"NONE",
	"TRACE",
	"DEBUG",
	"WARN",
	"INFO",
	"ERROR",
	"FATAL"
};

const char *LogLevelStr(const int level)
{
	if (level >= NONE && level < _MAX_LEVEL)
	{
		return LogLevelStrings[level];
	}

	return "INVALID";
}

const char *trimFilePrefix(const char *path, const char *prefix)
{
	const char *tfilePtr = path, *filePtr = tfilePtr;
	tfilePtr = strstr(tfilePtr, prefix);
	if (tfilePtr)
	{
		filePtr = tfilePtr;
		tfilePtr = strchr(filePtr, '/');
		if (tfilePtr)
		{
			tfilePtr++;
			filePtr = tfilePtr;
		}
	}

	return filePtr;
}

const char *trimFunctionName(const char *func)
{
	const char *tfnPtr = func, *fnPtr = tfnPtr;
	tfnPtr = strchr(tfnPtr, '(');
	if (tfnPtr)
	{
		// strip off bits of the function name we don't care about.
		while (*tfnPtr != ' ' && *tfnPtr != '*' &&
				 *tfnPtr != '&' && *tfnPtr != ':' && tfnPtr > fnPtr)
		{
			tfnPtr--;
		}

		if (tfnPtr > func)
		{
			tfnPtr++;
		}

		fnPtr = tfnPtr;
	}

	return fnPtr;
}

#include <sstream>
#include <iomanip>

template<>
std::string AnyToString<size_t>(const size_t &s)
{
	std::stringstream sstr;

	sstr << std::fixed << s;

	return sstr.str();
}



template<>
std::string AnyToString<bool>(const bool &d)
{
	return d ? "true" : "false";
}

template<>
std::string AnyToString<int>(const int &d)
{
	char buff[1024];
	snprintf(buff, sizeof(buff), "%d", d);

	std::string str{buff};
	return str;
}

template<>
std::string AnyToString<unsigned int>(const unsigned int &d)
{
	char buff[1024];
	snprintf(buff, sizeof(buff), "%u", d);

	std::string str{buff};
	return str;
}

template<>
std::string AnyToString<char const*>(const char *const &d)
{
	return d;
}

template<>
std::string AnyToString<double>(const double &d)
{
	char buff[1024];
	snprintf(buff, sizeof(buff), "%-.2f", d);

	return {buff};
}

template<>
std::string AnyToString<float>(const float &d)
{
	char buff[1024];
	snprintf(buff, sizeof(buff), "%-.2f", d);

	return {buff};
}


template <>
std::string AnyToString<std::string>(const std::string &str)
{
	return str;
}

/**
 * Pulls out base function name, stripping out parameters and truncating at <code>outLength</code> bytes
 * starting from the end.
 *
 * @param orig original function name
 * @param origLength length of <code>orig</code> buffer
 * @param out destination buffer
 * @param outLength length of <code>out</code> buffer
 * @return <code>out</code> buffer
 */
char *yankFunctionName(char *dest, int destLength, const char *src, int srcLength)
{
	const char *tptr = src;
	int i = 0;

	bool haveOperator = strncmp(src, "operator", 8) == 0;

	while (i < srcLength && (*tptr != '(' || haveOperator))
	{
		if (*tptr == '(' && haveOperator)
		{
			haveOperator = false;
		}

		tptr++;
		i++;
	}

	int fnLen = 0, fnStart = 0;
	//if(tptr > src) {
	//  tptr--;
	//}

	const char *eptr = src + srcLength - 5;
	int trimEnd = 0;
	if (strcmp(eptr, "const") == 0)
	{
		trimEnd = 5;
	}

	fnLen = (int) (tptr - src - trimEnd);
	fnStart = fnLen > destLength ? fnLen - destLength + 1 : 0;

	strncpy(dest, src + fnStart, (size_t) std::min(destLength - 1, fnLen));

	return dest;
}


