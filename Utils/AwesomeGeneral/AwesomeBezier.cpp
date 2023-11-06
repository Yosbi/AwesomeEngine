#include "AwesomeBezier.h"

AwesomeBezier::AwesomeBezier()
{
}

AwesomeBezier::~AwesomeBezier()
{
}

AWEVector AwesomeBezier::Update(float fElapsedTime)
{
    m_fTotalTime += fElapsedTime;
    float percentage = m_fTotalTime / m_fTotalTimeAnim;
    if (percentage > 1.0f) {
        m_bAnimationFinished = true;
    }

    int n = m_vcPoints.size() - 1;
    std::vector<AWEVector> tempPoints = m_vcPoints;

    for (int k = 1; k <= n; ++k) {
        for (int i = 0; i <= n - k; ++i) {
            tempPoints[i].SetX( (1 - percentage) * tempPoints[i].GetX() + percentage * tempPoints[i + 1].GetX());
            tempPoints[i].SetY( (1 - percentage) * tempPoints[i].GetY() + percentage * tempPoints[i + 1].GetY());
            tempPoints[i].SetZ( (1 - percentage) * tempPoints[i].GetZ() + percentage * tempPoints[i + 1].GetZ());
        }
    }

    return tempPoints[0];
}

void AwesomeBezier::Set(std::vector<AWEVector> vcPoints, float totalTimeAnim)
{
    m_vcPoints = vcPoints;

    m_fTotalTimeAnim = totalTimeAnim;
    m_bAnimationFinished = false;
    m_fTotalTime = 0.0f;
}
