//-----------------------------------------------------------------------
// AwesomeFreeCam.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 22/04/2023
//-----------------------------------------------------------------------
// Desc: Implements a very basic camera that is unaware of its surrounding
//       but able to move totally free like in zero - g space.
#ifndef AWEFREECAM_H
#define AWEFREECAM_H
#pragma once

#include "AwesomeGeneral.h"

class AwesomeFreeCam : public AwesomeBaseCam {
public:
    AwesomeFreeCam(void);
    virtual ~AwesomeFreeCam(void);

    virtual void Update(float fElapsedTime);

    // use this functions to influence movement
    void AddRotationSpeed(float sx, float sy, float sz);
    void SetRotationSpeed(float sx, float sy, float sz);
    void SetRotationSpeedX(float f) { m_fPitchSpd = f; }
    void SetRotationSpeedY(float f) { m_fYawSpd = f; }
    void SetRotationSpeedZ(float f) { m_fRollSpd = f; }
    void AddThrust(float fThrust) { m_fThrust += fThrust; }
    void SetThrust(float fThrust) { m_fThrust = fThrust; }

    // set attributes directly (avoid this except for initialization)
    void SetRotation(float rx, float ry, float rz);
    void SetPos(AWEVector& vc) { memcpy(&m_vcPos, &vc, sizeof(AWEVector)); }
    void SetRight(AWEVector& vc) { memcpy(&m_vcRight, &vc, sizeof(AWEVector)); }
    void SetUp(AWEVector& vc) { memcpy(&m_vcUp, &vc, sizeof(AWEVector)); }
    void SetDir(AWEVector& vc) { memcpy(&m_vcDir, &vc, sizeof(AWEVector)); }
};

#endif
