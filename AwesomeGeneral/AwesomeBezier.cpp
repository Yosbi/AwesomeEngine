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
            tempPoints[i].x = (1 - percentage) * tempPoints[i].x + percentage * tempPoints[i + 1].x;
            tempPoints[i].y = (1 - percentage) * tempPoints[i].y + percentage * tempPoints[i + 1].y;
            tempPoints[i].z = (1 - percentage) * tempPoints[i].z + percentage * tempPoints[i + 1].z;
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
