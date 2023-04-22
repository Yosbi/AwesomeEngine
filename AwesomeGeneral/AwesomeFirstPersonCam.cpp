//-----------------------------------------------------------------------
// AwesomeFreeCam.cpp
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 22/04/2023
//-----------------------------------------------------------------------
#include "AwesomeGeneral.h"

//--------------------------------------------------------------------
// Name: Constructor
// Desc: Just initialize some values.
//--------------------------------------------------------------------
// Just initialize some values.
AwesomeFirstPersonCam::AwesomeFirstPersonCam(void) {
    AwesomeBaseCam::Init();
    m_fSpeed = 0;
    m_fSlide = 0;
} 

//--------------------------------------------------------------------
// Name: Destructor
// Desc:
//--------------------------------------------------------------------
AwesomeFirstPersonCam::~AwesomeFirstPersonCam(void) {

} 

//--------------------------------------------------------------------
// Name: SetRotation
// Desc: Set rotation values and recalculate local vectors.
//--------------------------------------------------------------------
void AwesomeFirstPersonCam::SetRotation(float rx, float ry, float rz) {
    m_fRotX = rx;
    m_fRotY = ry;
    m_fRotZ = rz;

    RecalcAxes();
} 

//--------------------------------------------------------------------
// Name: Update
// Desc: Calculate movement based on behavior model and attribute values.
//      Input parameter is elapsed time since last frame.
//--------------------------------------------------------------------
void AwesomeFirstPersonCam::Update(float fET) {
    AWEVector vcS;
    // add rotation speed
    m_fRotX += (m_fPitchSpd * fET);
    m_fRotY += (m_fYawSpd * fET);
    m_fRotZ += (m_fRollSpd * fET);

    // recalculate axes
    RecalcAxes();

    // calculate velocity vectors
    m_vcV = m_vcDir * m_fSpeed * fET;
    vcS = m_vcRight * m_fSlide * fET;

    // move position according to velocity vector
    m_vcPos += m_vcV + vcS;
}

//--------------------------------------------------------------------
// Name: RecalcAxes
// Desc: Recalc the axes but limiting the y to more or less 80 to -80 
//      degrees
//--------------------------------------------------------------------
void AwesomeFirstPersonCam::RecalcAxes(void) {
    AWEMatrix  mat;

    static float f2PI = 6.283185f;

    // keep in range of 360 degree
    if (m_fRotY > f2PI) m_fRotY -= f2PI;
    else if (m_fRotY < -f2PI) m_fRotY += f2PI;

    // dont look up/down more than 80 degrees
    if (m_fRotX > 1.4f) m_fRotX = 1.4f;
    else if (m_fRotX < -1.4f) m_fRotX = -1.4f;

    // initialize axes
    m_vcRight = AWEVector(1.0f, 0.0f, 0.0f);
    m_vcUp = AWEVector(0.0f, 1.0f, 0.0f);
    m_vcDir = AWEVector(0.0f, 0.0f, 1.0f);

    // rotate around Y axis
    mat.RotaArbi(m_vcUp, m_fRotY);
    m_vcRight = m_vcRight * mat;
    m_vcDir = m_vcDir * mat;

    // rotate around X axis
    mat.RotaArbi(m_vcRight, m_fRotX);
    m_vcUp = m_vcUp * mat;
    m_vcDir = m_vcDir * mat;

    // fight rounding errors
    m_vcDir.Normalize();
    m_vcRight = m_vcUp.Cross(m_vcDir);
    m_vcRight.Normalize();
    m_vcUp = m_vcDir.Cross(m_vcRight);
    m_vcUp.Normalize();
} 

//--------------------------------------------------------------------
// Name: GetRotation
// Desc: Return rotation in the float values parameters
//--------------------------------------------------------------------
void AwesomeFirstPersonCam::GetRotation(float& pfX, float& pfY, float& pfZ) {
    pfX = m_fRotX;
    pfY = m_fRotY;
    pfZ = m_fRotZ;
}

//--------------------------------------------------------------------
// Name: GetRotation
// Desc: Return rotation in a vector form
//--------------------------------------------------------------------
AWEVector AwesomeFirstPersonCam::GetRotation(void) {
    return AWEVector(m_fRotX,
        m_fRotY,
        m_fRotZ);
}