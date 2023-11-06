//-----------------------------------------------------------------------
// AwesomeBaseCam.cpp
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 22/04/2023
//-----------------------------------------------------------------------
#include "AwesomeGeneral.h"

//--------------------------------------------------------------------
// Name: Constructor
// Desc: Just initialize some values.
//--------------------------------------------------------------------
AwesomeBaseCam::AwesomeBaseCam(void) {
	Init();
}

//--------------------------------------------------------------------
// Name: Destructor
// Desc: 
//--------------------------------------------------------------------
AwesomeBaseCam::~AwesomeBaseCam(void) {
} 

//--------------------------------------------------------------------
// Name: Init()
// Desc: Init values
//--------------------------------------------------------------------
void AwesomeBaseCam::Init(void) {
	m_vcPos.Set(0.0f, 0.0f, 0.0f);
	m_vcRight.Set(1.0f, 0.0f, 0.0f);
	m_vcUp.Set(0.0f, 1.0f, 0.0f);
	m_vcDir.Set(0.0f, 0.0f, 1.0f);
	m_vcV.Set(0.0f, 0.0f, 0.0f);
	m_fRotX = m_fRotY = m_fRotZ = m_fThrust = 0.0f;
	m_fRollSpd = m_fPitchSpd = m_fYawSpd = 0.0f;
	m_Quat.SetX(0.0f); m_Quat.SetY(0.0f); m_Quat.SetZ(0.0f);
	m_Quat.SetW(1.0f);
}

//--------------------------------------------------------------------
// Name: RecalcAxes()
// Desc: Recalculate the orientation of the local axes according to the
// current rotation values around the axes.
//--------------------------------------------------------------------
void AwesomeBaseCam::RecalcAxes(void) {
	AWEQuat    qFrame;
	AWEMatrix  mat;
	
	//OutputDebugStringA((std::to_string(m_fRotX) + "\n").c_str());

	// keep in range of 360 degree
	if (m_fRotX > AWE2PI)
		m_fRotX -= AWE2PI;
	else if (m_fRotX < -AWE2PI)
		m_fRotX += AWE2PI;

	if (m_fRotY > AWE2PI)
		m_fRotY -= AWE2PI;
	else if (m_fRotY < -AWE2PI)
		m_fRotY += AWE2PI;

	if (m_fRotZ > AWE2PI)
		m_fRotZ -= AWE2PI;
	else if (m_fRotZ < -AWE2PI)
		m_fRotZ += AWE2PI;

	// build new quaternion for this rotation
	qFrame.MakeFromEuler(m_fRotX, m_fRotY, m_fRotZ);

	// add to existing rotation
	m_Quat *= qFrame;

	// extract the local axes
	m_Quat.GetMatrix(mat);

	m_vcRight.SetX(mat.GetElement(1, 1));
	m_vcRight.SetY(mat.GetElement(2, 1));
	m_vcRight.SetZ(mat.GetElement(3, 1));

	m_vcUp.SetX(mat.GetElement(1, 2));
	m_vcUp.SetY(mat.GetElement(2, 2));
	m_vcUp.SetZ(mat.GetElement(3, 2));

	m_vcDir.SetX(mat.GetElement(1, 3));
	m_vcDir.SetY(mat.GetElement(2, 3));
	m_vcDir.SetZ(mat.GetElement(3, 3));
} 