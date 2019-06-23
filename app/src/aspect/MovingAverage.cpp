//
// Created by moose on 05/04/19.
//

#include "aspect/MovingAverage.h"


MovingAverage::MovingAverage(unsigned int samples)
		: m_maxSampleCount(samples)
		, m_sampleCount(0)
		, m_currentSample(0)
		, m_total(0.0f)
		, m_samples(samples)
{
}

void MovingAverage::addSample(float sample)
{
	if (m_sampleCount == m_maxSampleCount)
		m_total -= m_samples.at(m_currentSample);
	else
		++m_sampleCount;

	m_samples[m_currentSample] = sample;
	m_total += sample;
	++m_currentSample;
	if (m_currentSample >= m_maxSampleCount)
		m_currentSample = 0;
}

float MovingAverage::average() const
{
	return m_sampleCount ? m_total / static_cast<float>(m_sampleCount) : 0.0f;
}
