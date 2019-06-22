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
#include <QtCore/QRectF>
#include "LogUtils.h"

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

#include <QString>
#include <QtCore/QDebug>

template<>
std::string AnyToString<QRectF>(const QRectF &d)
{
	QString str;
	QDebug debug(&str);

	debug << d;
	return str.toStdString();
}

template<>
std::string AnyToString<QPointF>(const QPointF &d)
{
	QString str;
	QDebug debug(&str);

	debug << d;
	return str.toStdString();
}

#include <Qt3DCore/QTransform>
template<>
std::string AnyToString<Qt3DCore::QTransform*>(Qt3DCore::QTransform * const &d)
{
	QString str;
	QDebug debug(&str);


	debug << "QTransform{";
	debug << "translate=" << d->translation() << " scale=" << d->scale();
	debug << "}";

	return str.toStdString();
}

#include <QMatrix4x4>
template<>
std::string AnyToString<QMatrix4x4>(const QMatrix4x4 &d)
{
	QString str;
	QDebug debug(&str);

	debug << d;

	return str.toStdString();
}

#include <QVector3D>
#include <Qt3DCore/QSceneChange>
#include <Qt3DCore/QPropertyValueAddedChange>


template<>
std::string AnyToString<QVector3D>(const QVector3D &d)
{
	std::stringstream sstr;

	sstr <<  std::fixed << "QVector3D{" << d.x() << ", " << d.y() << ", " << d.z() << "}";

	return sstr.str();
}

template<>
std::string AnyToString<QVector4D>(const QVector4D &d)
{
	QString str;
	QDebug debug(&str);

	debug << d;

	return str.toStdString();
}

#include "quadtree/QuadTreeNodeItem.h"
template<> std::string AnyToString(const std::vector<QuadTreeNodeItem*> &d)
{
	std::stringstream sstr;

	bool haveItem = false;
	for (auto &item: d)
	{
		if (haveItem)
		{
			sstr << ", ";
		}

		sstr << item->toString();

		haveItem = true;
	}

	return sstr.str();
}

std::ostream & operator << (std::ostream &out, const QVector3D &c)
{
	out << std::fixed << "QVector3D{" << c.x() << ", " << c.y() << ", " << c.z() << "}";
	return out;
}



#include <QPickLineEvent>
template<>
std::string AnyToString<Qt3DRender::QPickLineEvent *>(Qt3DRender::QPickLineEvent * const &e)
{
	QString str;
	QDebug debug(&str);

	debug << "pos=" << e->position() << " edge=" << e->edgeIndex() << "vtxs=" << e->vertex1Index() << "," << e->vertex2Index();

	return str.toStdString();
}

#include <Qt3DInput/QMouseEvent>
template<>
std::string AnyToString<Qt3DInput::QMouseEvent*>(Qt3DInput::QMouseEvent * const &ev)
{
	std::stringstream sstr;
	sstr << "mouse: pos=" << ev->x() << "x" << ev->y() << " held=" << ev->wasHeld() << " buttons=" << ev->buttons();

	return sstr.str();
}

#include <QtCore/QPoint>
template<>
std::string AnyToString<QPoint>(const QPoint &pt)
{
	QString str;
	QDebug debug(&str);

	debug << pt;

	return str.toStdString();
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

const char *changeFlagString(const Qt3DCore::ChangeFlag flag)
{
	switch (flag)
	{
//		NodeCreated             = 1 << 0,
//		NodeDeleted             = 1 << 1,
//		PropertyUpdated         = 1 << 2,
//		PropertyValueAdded      = 1 << 3,
//		PropertyValueRemoved    = 1 << 4,
//		ComponentAdded          = 1 << 5,
//		ComponentRemoved        = 1 << 6,
//		CommandRequested        = 1 << 7,
//		CallbackTriggered       = 1 << 8,

		case Qt3DCore::NodeCreated:
			return "NodeCreated";

		case Qt3DCore::NodeDeleted:
			return "NodeDeleted";

		case Qt3DCore::PropertyUpdated:
			return "PropertyUpdated";

		case Qt3DCore::PropertyValueAdded:
			return "PropertyValueAdded";

		case Qt3DCore::PropertyValueRemoved:
			return "PropertyValueRemoved";

		case Qt3DCore::ComponentAdded:
			return "ComponentAdded";

		case Qt3DCore::ComponentRemoved:
			return "ComponentRemoved";

		case Qt3DCore::CommandRequested:
			return "CommandRequested";

		case Qt3DCore::CallbackTriggered:
			return "CallbackTriggered";
	}

	return "UNK";
}

#include <iostream>
#include <Qt3DCore/QSceneChange>
#include <sstream>
#include <Qt3DCore/QPropertyUpdatedChange>

template<>
std::string AnyToString<Qt3DCore::QSceneChangePtr>(const Qt3DCore::QSceneChangePtr &e)
{
	std::stringstream ss;

	std::string type = changeFlagString(e->type());

	Qt3DCore::QNodeId id = e->subjectId();

	ss << "SceneChange: " << id << ": " << type;

	if (e->type() == Qt3DCore::PropertyUpdated)
	{
		const auto change = qSharedPointerCast<Qt3DCore::QPropertyUpdatedChange>(e);
		//QVariant variant = change->value();
		//QString string = variant.toString();

		ss << " " << change->propertyName() ;//<< " " << string.toStdString();
		//ss << " asdf";
	}

	return ss.str();
}

template <>
std::string AnyToString<Qt3DCore::ChangeFlag>(const Qt3DCore::ChangeFlag &flag)
{
	const char *str = changeFlagString(flag);
	//std::cout << " flag: " << str;

	return str;
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


