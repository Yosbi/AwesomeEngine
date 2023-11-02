#include "AwesomeLinearInterpolation.h"

AwesomeLinearInterpolation::AwesomeLinearInterpolation()
{
}

AwesomeLinearInterpolation::~AwesomeLinearInterpolation()
{
}

AWEVector AwesomeLinearInterpolation::Update(float fElapsedTime)
{
    m_fTotalTime += fElapsedTime;
    if (m_fTotalTime > 1.0f) {
        m_fTotalTime = 1.0f;
        m_bAnimationFinished = true;
    }
        
    AWEVector result;
    result.x = (1 - m_fTotalTime) * m_vcStart.x + m_fTotalTime * m_vcEnd.x;
    result.y = (1 - m_fTotalTime) * m_vcStart.y + m_fTotalTime * m_vcEnd.y;
    result.z = (1 - m_fTotalTime) * m_vcStart.z + m_fTotalTime * m_vcEnd.z;

    return result;
}

void AwesomeLinearInterpolation::Set(AWEVector vcStart, AWEVector vcEnd)
{
	m_vcStart = vcStart;
	m_vcEnd = vcEnd;

    m_bAnimationFinished = false;
    m_fTotalTime = 0.0f;
}
