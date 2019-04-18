//
// Created by moose on 05/04/19.
//

#ifndef CITYSIM_FPSMONITORCOMPONENT_H
#define CITYSIM_FPSMONITORCOMPONENT_H


#include <Qt3DCore/QComponent>

struct FpsMonitorData
{
	int rollingMeanFrameCount;
};

class FpsMonitorComponent : public Qt3DCore::QComponent
{
	Q_OBJECT
		Q_PROPERTY(int rollingMeanFrameCount READ rollingMeanFrameCount WRITE setRollingMeanFrameCount NOTIFY rollingMeanFrameCountChanged)
		Q_PROPERTY(float framesPerSecond READ framesPerSecond NOTIFY framesPerSecondChanged)

	public:
		explicit FpsMonitorComponent(Qt3DCore::QNode *parent = nullptr);

		float framesPerSecond() const;
		int rollingMeanFrameCount() const;

	public slots:
		void setRollingMeanFrameCount(int rollingMeanFrameCount);

	signals:
		void framesPerSecondChanged(float framesPerSecond);
		void rollingMeanFrameCountChanged(int rollingMeanFrameCount);

	protected:
		void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) override;

	private:
		Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;

		float m_framesPerSecond;
		int m_rollingMeanFrameCount;
};


#endif //CITYSIM_FPSMONITORCOMPONENT_H
