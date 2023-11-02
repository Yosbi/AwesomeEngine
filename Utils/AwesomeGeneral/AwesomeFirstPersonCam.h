//-----------------------------------------------------------------------
// AwesomeFirstPersonCam.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 22/04/2023
//-----------------------------------------------------------------------
// Desc: Implements a first person camera with a Y range of 80 to -80 degrees

#ifndef AWEFIRSTPERSONCAMERA_H
#define AWEFIRSTPERSONCAMERA_H

#include "AwesomeGeneral.h"

class AwesomeFirstPersonCam : public AwesomeBaseCam {
public:
    AwesomeFirstPersonCam(void);
    virtual ~AwesomeFirstPersonCam(void);

    virtual void Update(float fElapsedTime);

    void      GetRotation(float& pfX, float& pfY, float& pfZ);
    AWEVector GetRotation(void);

    void SetRotationSpeedX(float f) { m_fPitchSpd = f; }
    void SetRotationSpeedY(float f) { m_fYawSpd = f; }
    void SetSpeed(float a) { m_fSpeed = a; }
    void SetSlideSpeed(float a) { m_fSlide = a; }

    // set attributes directly (avoid this except for initialization)
    void SetRotation(float rx, float ry, float rz);
    void SetPos(AWEVector& vc) { memcpy(&m_vcPos, &vc, sizeof(AWEVector)); }
    void SetRight(AWEVector& vc) { memcpy(&m_vcRight, &vc, sizeof(AWEVector)); }
    void SetUp(AWEVector& vc) { memcpy(&m_vcUp, &vc, sizeof(AWEVector)); }
    void SetDir(AWEVector& vc) { memcpy(&m_vcDir, &vc, sizeof(AWEVector)); }

private:
    float m_fSpeed;
    float m_fSlide;

    void RecalcAxes(void);
};

#endif // !AWEFIRSTPERSONCAMERA_H