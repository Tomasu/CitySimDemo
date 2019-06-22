//
// Created by moose on 05/04/19.
//

#include <Qt3DCore/QPropertyUpdatedChange>
#include "FpsMonitorBackend.h"
#include "FpsMonitorComponent.h"
#include "MovingAverage.h"


FpsMonitorBackend::FpsMonitorBackend()
		: Qt3DCore::QBackendNode(Qt3DCore::QBackendNode::ReadWrite),
		m_rollingMeanFrameCount{0}
{
}

void FpsMonitorBackend::addFpsSample(float fpsSample)
{
	//qDebug() << "addFpsSample: " << fpsSample;

	// Update the moving average
	m_average.addSample(fpsSample);

	// Calculate the average and notify the frontend
	const auto meanFps = m_average.average();
	auto e = Qt3DCore::QPropertyUpdatedChangePtr::create(peerId());
	e->setDeliveryFlags(Qt3DCore::QSceneChange::Nodes);
	e->setPropertyName("framesPerSecond");
	e->setValue(QVariant::fromValue(meanFps));
	notifyObservers(e);
}

void FpsMonitorBackend::initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change)
{
	const auto typedChange = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<FpsMonitorData>>(change);
	const auto &data = typedChange->data;
	m_rollingMeanFrameCount = data.rollingMeanFrameCount;
	m_average = MovingAverage(m_rollingMeanFrameCount);
}

void FpsMonitorBackend::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
	if (e->type() == Qt3DCore::PropertyUpdated) {
		const auto change = qSharedPointerCast<Qt3DCore::QPropertyUpdatedChange>(e);
		qDebug() << change->propertyName();
		if (change->propertyName() == QByteArrayLiteral("rollingMeanFrameCount")) {
			const auto newValue = change->value().toInt();
			if (newValue != m_rollingMeanFrameCount) {
				m_rollingMeanFrameCount = newValue;
				m_average = MovingAverage(newValue);
			}
			return;
		}
	}
	QBackendNode::sceneChangeEvent(e);
}
