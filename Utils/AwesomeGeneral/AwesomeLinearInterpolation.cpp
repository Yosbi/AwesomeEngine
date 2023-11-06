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
    result.SetX( (1 - m_fTotalTime) * m_vcStart.GetX() + m_fTotalTime * m_vcEnd.GetX());
    result.SetY( (1 - m_fTotalTime) * m_vcStart.GetY() + m_fTotalTime * m_vcEnd.GetY());
    result.SetZ( (1 - m_fTotalTime) * m_vcStart.GetZ() + m_fTotalTime * m_vcEnd.GetZ());

    return result;
}

void AwesomeLinearInterpolation::Set(AWEVector vcStart, AWEVector vcEnd)
{
	m_vcStart = vcStart;
	m_vcEnd = vcEnd;

    m_bAnimationFinished = false;
    m_fTotalTime = 0.0f;
}
