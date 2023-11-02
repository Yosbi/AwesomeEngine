//-------------------------------------------------------------------------------
// AwesomeInput.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 20/04/2023
//-----------------------------------------------------------------------
//Desc: Defines the static lib to get access to the dll of the imput module
//--------------------------------------------------------------------------
#ifndef AWESOMEINPUT_H
#define AWESOMEINPUT_H
#pragma once

//-----------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------
#include "AwesomeInputDevice.h"

//-----------------------------------------------------------------------
// Name: AwesomeInput(Class)
// Desc: Class used to get a object to the input module
//-----------------------------------------------------------------------
class AwesomeInput
{
public:
	AwesomeInput(HINSTANCE hInst);
	~AwesomeInput(void);

	HRESULT          CreateDevice(void);
	LPAWEINPUTDEVICE GetDevice(void) { return m_pDevice; }
	HINSTANCE        GetModule(void) { return m_hDLL; }
	void             Release(void);

private:
	AwesomeInputDevice* m_pDevice;
	HINSTANCE			m_hInst;
	HMODULE				m_hDLL;
};

typedef class AwesomeInput* LPAWEINPUT;


#endif