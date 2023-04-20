//-------------------------------------------------------------------------------
// AweKeyboard.cpp
// Yosbi Alves
// yosbi@outlook.com
// Created on 20/04/2023
//-----------------------------------------------------------------------
//Desc: The implemetation of the keyboard class
//--------------------------------------------------------------------------
#include "AweDXInput.h"	// class definition

//-----------------------------------------------------------------------
// Name: YKeyboard
// Desc: Constructor
//-----------------------------------------------------------------------
AweKeyboard::AweKeyboard(LPDIRECTINPUT8 pDI, HWND hWnd)
{
	Create(pDI, hWnd);
}

//-----------------------------------------------------------------------
// Name: AweKeyboard
// Desc: Destructor
//-----------------------------------------------------------------------
AweKeyboard::~AweKeyboard(void)
{
	Release();
}

//-----------------------------------------------------------------------
// Name: Init
// Desc: Initializes the keyboard device.
//-----------------------------------------------------------------------
HRESULT AweKeyboard::Init(void)
{
	if (FAILED(CrankUp(GUID_SysKeyboard, &c_dfDIKeyboard)))
		return E_FAIL;

	// Clear out the structs
	memset(m_Keys, 0, sizeof(m_Keys));
	memset(m_KeysOld, 0, sizeof(m_KeysOld));

	// Acquire the device to make it work
	m_pDevice->Acquire();
	return S_OK;
}

//-----------------------------------------------------------------------
// Name: Update
// Desc: Update the keyboard device.
//----------------------------------------------------------------------
HRESULT AweKeyboard::Update(void)
{
	// Copy keyboard state from last frame
	memcpy(m_KeysOld, m_Keys, sizeof(m_Keys));

	// Try to get the data from the keyboard
	if (FAILED(GetData(IDV_KEYBOARD, &m_Keys[0], NULL)))
	{
		return E_FAIL;
	}
	return S_OK;
}

//-----------------------------------------------------------------------
// Name: IsPressed
// Desc: Check if the given key is pressed. Use WinAPI VK_ defines here.
//----------------------------------------------------------------------
bool AweKeyboard::IsPressed(UINT nID)
{
	// Translate virtual code to scan code
	if (m_Keys[nID] & 0x80)
		return true;
	return false;
}

//-----------------------------------------------------------------------
// Name: IsReleased
// Desc: Check if the given key is released. Use WinAPI VK_ defines here.
//----------------------------------------------------------------------
bool AweKeyboard::IsReleased(UINT nID)
{
	// Translate virtual code to scan code
	if ((m_KeysOld[nID] & 0x80) && !(m_Keys[nID] & 0x80))
		return true;
	return false;
}