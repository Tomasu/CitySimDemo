//
// Created by moose on 05/04/19.
//

#ifndef CITYSIM_MOVINGAVERAGE_H
#define CITYSIM_MOVINGAVERAGE_H

#include <QtCore/QVector>

class MovingAverage
{
	public:
		explicit MovingAverage(unsigned int samples = 3);

		void addSample(float sample);
		float average() const;

	private:
		unsigned int m_maxSampleCount;
		unsigned int m_sampleCount;
		unsigned int m_currentSample;
		float m_total;
		QVector<float> m_samples;
};


#endif //CITYSIM_MOVINGAVERAGE_H
