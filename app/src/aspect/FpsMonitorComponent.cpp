//
// Created by moose on 05/04/19.
//

#include <Qt3DCore/QPropertyUpdatedChange>
#include "FpsMonitorComponent.h"


FpsMonitorComponent::FpsMonitorComponent(Qt3DCore::QNode *parent)
		: Qt3DCore::QComponent(parent)
		, m_framesPerSecond{0.0f}
		, m_rollingMeanFrameCount{5}
{
}

float FpsMonitorComponent::framesPerSecond() const
{
	return m_framesPerSecond;
}

int FpsMonitorComponent::rollingMeanFrameCount() const
{
	return m_rollingMeanFrameCount;
}

void FpsMonitorComponent::setRollingMeanFrameCount(int rollingMeanFrameCount)
{
	if (m_rollingMeanFrameCount == rollingMeanFrameCount)
		return;

	m_rollingMeanFrameCount = rollingMeanFrameCount;
	emit rollingMeanFrameCountChanged(m_rollingMeanFrameCount);
}

void FpsMonitorComponent::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change)
{
	if (change->type() == Qt3DCore::PropertyUpdated) {
		const auto e = qSharedPointerCast<Qt3DCore::QPropertyUpdatedChange>(change);
		if (e->propertyName() == QByteArrayLiteral("framesPerSecond")) {
			const auto fps = e->value().toFloat();
			if (!qFuzzyCompare(fps, m_framesPerSecond)) {
				m_framesPerSecond = fps;
				const auto wasBlocked = blockNotifications(true);
				emit framesPerSecondChanged(fps);
				blockNotifications(wasBlocked);
			}
			return;
		}
	}
	else if (change->type() == Qt3DCore::NodeCreated)
	{
		qDebug() << " node created: " << change->subjectId();
	}

	QComponent::sceneChangeEvent(change);
}

Qt3DCore::QNodeCreatedChangeBasePtr FpsMonitorComponent::createNodeCreationChange() const
{
	auto creationChange = Qt3DCore::QNodeCreatedChangePtr<FpsMonitorData>::create(this);
	auto &data = creationChange->data;
	data.rollingMeanFrameCount = m_rollingMeanFrameCount;
	return creationChange;
}
