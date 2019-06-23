#include "util/LogUtils.h"

#include <QRectF>

#include <QString>
#include <QtCore/QDebug>

#include <sstream>

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
