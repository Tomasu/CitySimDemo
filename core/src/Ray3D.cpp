
#include "core/Ray3D.h"
#include "util/Plane3D.h"

#include <QDebug>

/*!
    \internal
    \class Ray3D
    \inmodule Qt3DRender
    \brief The Ray3D class defines a directional line in 3D space extending through an origin point.
    \since 5.5
    \ingroup qt3d
    \ingroup qt3d::math

    A ray is defined by the origin() point and the direction() vector.
    Rays are infinite in length, extending out from origin() in
    both directions.  If the direction() is zero length, then the
    behavior of the class is undefined.

    A ray can be thought of as a one-dimensional co-ordinate system.
    If the co-ordinate is \b t then the origin() point is at
    \b t = 0, the point origin() + direction() is at \b t = 1,
    and the point origin() - direction() is at \b t = -1.
    The point() method can be used to obtain the position of a point
    within this one-dimensional co-ordinate system. The projectedDistance()
    method can be used to convert a point into a value in this
    one-dimensional co-ordinate system.
*/

/*!
    \fn Ray3D::Ray3D()

    Construct a default ray with an origin() of (0, 0, 0), a
    direction() of (0, 0, 1) and a distance of 1.
*/
Ray3D::Ray3D()
    : m_direction(0.0f, 0.0f, 1.0f)
    , m_distance(1.0f)
{
}

/*!
    \fn Ray3D::Ray3D(const QVector3D &origin, const QVector3D &direction, float distance)

    Construct a ray given its defining \a origin, \a direction and \a distance.
    The \a direction does not need to be normalized.

    To construct a ray that passes through two points, use the following:

    \code
    Ray3D thruAB(pointA, pointB - pointA);
    \endcode
*/
Ray3D::Ray3D(const QVector3D &origin, const QVector3D &direction, float distance)
    : m_origin(origin)
    , m_direction(direction)
    , m_distance(distance)
{}

Ray3D::~Ray3D()
{
}

/*!
    \fn QQVector3D Ray3D::origin() const

    Returns the origin of this ray.  The default value is (0, 0, 0).

    \sa setOrigin(), direction()
*/
QVector3D Ray3D::origin() const
{
    return m_origin;
}

/*!
    \fn void Ray3D::setOrigin(const QVector3D &value)

    Sets the origin point of this ray to \a value.

    \sa origin(), setDirection()
 */
void Ray3D::setOrigin(const QVector3D &value)
{
    m_origin = value;
}

/*!
    \fn QQVector3D Ray3D::direction() const

    Returns the direction vector of this ray.  The default value is (0, 0, 1).

    \sa setDirection(), origin()
*/
QVector3D Ray3D::direction() const
{
    return m_direction;
}

/*!
    \fn void Ray3D::setDirection(const QVector3D &direction)

    Sets the direction vector of this ray to \a direction.

    \sa direction(), setOrigin()
*/
void Ray3D::setDirection(const QVector3D &value)
{
    if (value.isNull())
        return;

    m_direction = value;
}

float Ray3D::distance() const
{
    return m_distance;
}

void Ray3D::setDistance(float distance)
{
    m_distance = distance;
}

QVector3D Ray3D::point(float t) const
{
    return m_origin + t * m_direction;
}

Ray3D &Ray3D::transform(const QMatrix4x4 &matrix)
{
    m_origin = matrix * m_origin;
    m_direction = matrix.mapVector(m_direction);

    return *this;
}

Ray3D Ray3D::transformed(const QMatrix4x4 &matrix) const
{
    return Ray3D(matrix * m_origin, matrix.mapVector(m_direction));
}

bool Ray3D::operator==(const Ray3D &other) const
{
    return m_origin == other.origin() && m_direction == other.direction();
}

bool Ray3D::operator!=(const Ray3D &other) const
{
    return !(*this == other);
}

/*!
    Returns \c true if \a point lies on this ray; \c false otherwise.
*/
bool Ray3D::contains(const QVector3D &point) const
{
    QVector3D  ppVec(point - m_origin);
    if (ppVec.isNull()) // point coincides with origin
        return true;
    const float dot = QVector3D ::dotProduct(ppVec, m_direction);
    if (qFuzzyIsNull(dot))
        return false;
    return qFuzzyCompare(dot*dot, ppVec.lengthSquared() * m_direction.lengthSquared());
}

/*!
    Returns \c true if \a ray lies on this ray; \c false otherwise. If true,
    this implies that the two rays are actually the same, but with
    different origin() points or an inverted direction().
*/
bool Ray3D::contains(const Ray3D &ray) const
{
    const float dot = QVector3D ::dotProduct(m_direction, ray.direction());
    if (!qFuzzyCompare(dot*dot, m_direction.lengthSquared() * ray.direction().lengthSquared()))
        return false;
    return contains(ray.origin());
}

/*!
    \fn QQVector3D Ray3D::point(float t) const

    Returns the point on the ray defined by moving \a t units
    along the ray in the direction of the direction() vector.
    Note that \a t may be negative in which case the point returned
    will lie behind the origin() point with respect to the
    direction() vector.

    The units for \a t are defined by direction().  The return value
    is precisely origin() + t * direction().

    \sa projectedDistance(), distance()
*/

/*!
    Returns the number of direction() units along the ray from origin()
    to \a point.  Essentially, this function computes the value t, where
    \a point = origin() + t * direction().  If \a point is not on the ray,
    then the closest point that is on the ray will be used instead.

    If the return value is positive, then \a point lies in front of
    the origin() with respect to the direction() vector.  If the return
    value is negative, then \a point lies behind the origin() with
    respect to the direction() vector.

    \sa point(), project()
*/
float Ray3D::projectedDistance(const QVector3D  &point) const
{
    Q_ASSERT(!m_direction.isNull());

    return QVector3D ::dotProduct(point - m_origin, m_direction) /
                m_direction.lengthSquared();
}

/*!
    Returns the projection of \a vector onto this ray.  In the
    following diagram, the dotted line is the ray, and V is the
    \a vector.  The return value will be the vector V':

    \image Ray3D-project.png

    \sa projectedDistance()
*/
QVector3D  Ray3D::project(const QVector3D &vector) const
{
    QVector3D  norm = m_direction.normalized();
    return QVector3D ::dotProduct(vector, norm) * norm;
}

/*!
    Returns the minimum distance from this ray to \a point, or equivalently
    the length of a line perpendicular to this ray which passes through
    \a point.  If \a point is on the ray, then this function will return zero.

    \sa point()
*/
float Ray3D::distance(const QVector3D  &point) const
{
    float t = projectedDistance(point);
    return (point - (m_origin + t * m_direction)).length();
}

QVector3D Ray3D::intersect(const Plane3D& plane)
{
	float d = QVector3D::dotProduct(plane.origin(), -plane.normal());
	float t = -(d + m_origin.z() * plane.normal().z() + m_origin.y() * plane.normal().y() + m_origin.x() * plane.normal().x()) / (m_direction.z() * plane.normal().z() + m_direction.y() * plane.normal().y() + m_direction.x() * plane.normal().x());
	return m_origin + t * m_direction;
}


/*!
    \fn Ray3D &Ray3D::transform(const QMatrix4x4 &matrix)

    Transforms this ray using \a matrix, replacing origin() and
    direction() with the transformed versions.

    \sa transformed()
*/

/*!
    \fn Ray3D Ray3D::transformed(const QMatrix4x4 &matrix) const

    Returns a new ray that is formed by transforming origin()
    and direction() using \a matrix.

    \sa transform()
*/

/*!
    \fn bool Ray3D::operator==(const Ray3D &other) const

    Returns \c true if this ray is the same as \a other; \c false otherwise.

    \sa operator!=()
*/

/*!
    \fn bool Ray3D::operator!=(const Ray3D &other) const

    Returns \c true if this ray is not the same as \a other; \c false otherwise.

    \sa operator==()
*/

/*!
    \fn bool qFuzzyCompare(const Ray3D &ray1, const Ray3D &ray2)
    \relates  Ray3D

    Returns \c true if \a ray1 and \a ray2 are almost equal; \c false
    otherwise.
*/

#ifndef QT_NO_DEBUG_STREAM

QDebug operator<<(QDebug dbg, const Ray3D &ray)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "Ray3D(origin("
        << ray.origin().x() << ", " << ray.origin().y() << ", "
        << ray.origin().z() << ") - direction("
        << ray.direction().x() << ", " << ray.direction().y() << ", "
        << ray.direction().z() << ") - distance(" << ray.distance() << "))";
    return dbg;
}

#endif

#ifndef QT_NO_DATASTREAM

/*!
    \relates Ray3D

    Writes the given \a ray to the given \a stream and returns a
    reference to the stream.
*/
QDataStream &operator<<(QDataStream &stream, const Ray3D &ray)
{
    stream << ray.origin();
    stream << ray.direction();
    if (stream.version() >= QDataStream::Qt_5_11)
        stream << ray.distance();
    return stream;
}

/*!
    \relates Ray3D

    Reads a 3D ray from the given \a stream into the given \a ray
    and returns a reference to the stream.
*/
QDataStream &operator>>(QDataStream &stream, Ray3D &ray)
{
    QVector3D origin, direction;
    float distance = 1.f;

    stream >> origin;
    stream >> direction;
    if (stream.version() >= QDataStream::Qt_5_11)
        stream >> distance;
    ray = Ray3D(QVector3D(origin), QVector3D(direction), distance);
    return stream;
}

#endif // QT_NO_DATASTREAM
