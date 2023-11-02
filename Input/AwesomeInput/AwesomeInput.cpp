//-------------------------------------------------------------------------------
// AwesomeInput.cpp
// Yosbi Alves
// yosbi@outlook.com
// Created on 20/04/2023
//-----------------------------------------------------------------------
//Desc: Implementation of the class YInput
//--------------------------------------------------------------------------
#include "AwesomeInput.h"

//-----------------------------------------------------------------------
// Name: AwesomeInput(Const)
// Desc: Constructor
//-----------------------------------------------------------------------
AwesomeInput::AwesomeInput(HINSTANCE hInst)
{
	m_hInst = hInst;
	m_pDevice = NULL;
	m_hDLL = NULL;
}

//-----------------------------------------------------------------------
// Name: ~AwesomeInput(Dest)
// Desc: Destructor
//-----------------------------------------------------------------------
AwesomeInput::~AwesomeInput(void)
{
	Release();
}

//-----------------------------------------------------------------------
// Name: CreateDevice
// Desc: Create the dll objects. This functions loads the appropriate dll.
//-----------------------------------------------------------------------
HRESULT AwesomeInput::CreateDevice(void)
{
	// load the DLL containing interface implementation
	m_hDLL = LoadLibraryEx(L"AweDXInput.dll", NULL, 0);
	if (!m_hDLL)
	{
		MessageBox(NULL, L"Loading AweDXInput.dll from lib failed.", L"Awesome Engine - error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	CREATEINPUTDEVICE _CreateInputDevice = 0;
	HRESULT hr;

	// Function pointer to dll's 'CreateInputDevice' function
	_CreateInputDevice = (CREATEINPUTDEVICE)GetProcAddress(m_hDLL, "CreateInputDevice");

	// Call dll's create function
	hr = _CreateInputDevice(m_hDLL, &m_pDevice);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"CreateInputDevice() from lib failed.", L"Awesome Engine - error", MB_OK | MB_ICONERROR);
		m_pDevice = NULL;
		return E_FAIL;
	}
	return S_OK;
}

//-----------------------------------------------------------------------
// Name: Release
// Desc: Clean up dll objects
//-----------------------------------------------------------------------
void AwesomeInput::Release(void)
{
	RELEASEINPUTDEVICE _ReleaseInputDevice = 0;
	HRESULT hr;

	if (m_hDLL)
	{
		// function pointer to dll 'ReleaseInputDevice' function
		_ReleaseInputDevice = (RELEASEINPUTDEVICE)GetProcAddress(m_hDLL, "ReleaseInputDevice");
	}
	// call dll's release function
	if (m_pDevice)
	{
		hr = _ReleaseInputDevice(&m_pDevice);
		if (FAILED(hr))
			m_pDevice = NULL;
	}
}
