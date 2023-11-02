//-----------------------------------------------------------------------
// AwesomeBaseCam.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 22/04/2023
//-----------------------------------------------------------------------
// Desc: Base class for all types of cameras to inherit from. This already
//       implements a very basic camera that is unaware of its surrounding
//       but able to move totally free like in zero - g space.
#ifndef AWEBASECAM_H
#define AWEBASECAM_H
#pragma once

#include "../../Math/AwesomeMath/AwesomeMath.h"

enum AWECAMERATYPE
{
    FREE_CAM,
    FIRST_PERSON_CAM,
    CUSTOM
};

class AwesomeBaseCam {
public:
    AwesomeBaseCam();
    virtual ~AwesomeBaseCam();

    virtual void Update(float fElapsedTime) = 0;

    AWEVector GetPos(void) { return m_vcPos; }
    AWEVector GetRight(void) { return m_vcRight; }
    AWEVector GetUp(void) { return m_vcUp; }
    AWEVector GetDir(void) { return m_vcDir; }
    AWEVector GetVelocity(void) { return m_vcV; }

    AWECAMERATYPE GetCameraType(void) { return m_CameraType; }

protected:
    AWEVector m_vcPos;      // position
    AWEVector m_vcRight;    // right vector
    AWEVector m_vcUp;       // up vector
    AWEVector m_vcDir;      // direction vector (look direction)
    AWEVector m_vcV;        // velocity vector (movement direction)
    AWEQuat   m_Quat;       // quaternion for rotation

    // rotation speed around local vectors
    float     m_fRollSpd;
    float     m_fPitchSpd;
    float     m_fYawSpd;

    float     m_fRollSpdMax;
    float     m_fPitchSpdMax;
    float     m_fYawSpdMax;

    // rotation value around local vectors
    float     m_fRotX;
    float     m_fRotY;
    float     m_fRotZ;

    // other stuff
    float m_fThrust;

    // protected methods
    virtual void RecalcAxes(void);
    virtual void Init(void);

    AWECAMERATYPE m_CameraType;
};

#endif


