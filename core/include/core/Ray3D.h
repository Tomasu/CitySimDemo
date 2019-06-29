
#ifndef CITYSIM_CORE_RAY3D_H
#define CITYSIM_CORE_RAY3D_H

#include <QVector3D>
#include <QMatrix4x4>
#include <QtCore/qmetatype.h>
#include <QtCore/qtypeinfo.h>

class Plane3D;

class Ray3D
{
	public:
		Ray3D();
		explicit Ray3D(const QVector3D &origin, const QVector3D &direction = QVector3D(0.0f, 0.0f, 1.0f), float distance = 1.0f);
		~Ray3D();

		QVector3D origin() const;
		void setOrigin(const QVector3D &value);

		QVector3D direction() const;
		void setDirection(const QVector3D &value);

		float distance() const;
		void setDistance(float distance);

		bool contains(const QVector3D &point) const;
		bool contains(const Ray3D &ray) const;

		QVector3D point(float t) const;
		float projectedDistance(const QVector3D &point) const;

		QVector3D project(const QVector3D &vector) const;

		float distance(const QVector3D &point) const;

		Ray3D &transform(const QMatrix4x4 &matrix);
		Ray3D transformed(const QMatrix4x4 &matrix) const;

		bool operator==(const Ray3D &other) const;
		bool operator!=(const Ray3D &other) const;

		bool isValid() const { return !m_direction.isNull() && !qFuzzyIsNull(m_distance); }

		QVector3D intersect(const Plane3D &plane);

	private:
		QVector3D m_origin;
		QVector3D m_direction;
		float m_distance;
};
Q_DECLARE_TYPEINFO(Ray3D, Q_MOVABLE_TYPE);


#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const Ray3D &ray);
#endif

#ifndef QT_NO_DATASTREAM
QDataStream &operator<<(QDataStream &stream, const Ray3D &ray);
QDataStream &operator>>(QDataStream &stream, Ray3D &ray);
#endif


inline bool qFuzzyCompare(const Ray3D &ray1, const Ray3D &ray2)
{
	return qFuzzyCompare(ray1.origin(), ray2.origin()) &&
	qFuzzyCompare(ray1.direction(), ray2.direction());
}

Q_DECLARE_METATYPE(Ray3D) // LCOV_EXCL_LINE

#endif // CITYSIM_CORE_RAY3D_H
