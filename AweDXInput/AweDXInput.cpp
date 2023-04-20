//-------------------------------------------------------------------------------
// AweDXInput.cpp
// Yosbi Alves
// yosbi@outlook.com
// Created on 20/04/2023
//-----------------------------------------------------------------------
//Desc: The implemetation of the imputdevice 
//--------------------------------------------------------------------------
#include "AweDXInput.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

//-----------------------------------------------------------
// DLL stuff implementation                                  
//-----------------------------------------------------------
BOOL WINAPI DllEntryPoint(HINSTANCE hDll, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason) {
		// called when we attach to the DLL
	case DLL_PROCESS_ATTACH:
		/* dll init/setup stuff */
		break;
	case DLL_PROCESS_DETACH:
		/* dll shutdown/release stuff */
		break;
	default:
		break;
	};

	return TRUE;
}

//-----------------------------------------------------------------------
// Name: CreateInputDevice
// Desc: Exported create function: Creates a new YInputDevice object
//-----------------------------------------------------------------------
HRESULT CreateInputDevice(HINSTANCE hDLL, AwesomeInputDevice** pDevice)
{
	if (!*pDevice)
	{
		*pDevice = new AweDXInput(hDLL);
		return S_OK;
	}
	return E_FAIL;
}

//-----------------------------------------------------------------------
// Name: ReleaseInputDevice
// Desc: Exported release function: Realeses the given YInputDevice object.
//-----------------------------------------------------------------------
HRESULT ReleaseInputDevice(AwesomeInputDevice** pDevice)
{
	if (!*pDevice)
	{
		return E_FAIL;
	}
	delete* pDevice;
	*pDevice = NULL;
	return S_OK;
}

//-----------------------------------------------------------
// AweDXInput class implementation                                
//-----------------------------------------------------------

//-----------------------------------------------------------------------
// Name: AweDXInput
// Desc: Constructor
//-----------------------------------------------------------------------
AweDXInput::AweDXInput(HINSTANCE hDLL)
{
	m_hDLL = hDLL;
	m_pDI = NULL;
	m_bRunning = false;
	m_pKB = NULL;
	m_pMouse = NULL;
	m_pJoy = NULL;
}

//-----------------------------------------------------------------------
// Name: AweDXInput
// Desc: Destructor
//-----------------------------------------------------------------------
AweDXInput::~AweDXInput()
{
	Release();
}


//-----------------------------------------------------------------------
// Name: Release
// Desc: Release all stuff.
//-----------------------------------------------------------------------
void AweDXInput::Release()
{
	if (m_pKB) {
		delete m_pKB;
		m_pKB = NULL;
	}

	if (m_pMouse) {
		delete m_pMouse;
		m_pMouse = NULL;
	}

	if (m_pJoy) {
		delete m_pJoy;
		m_pJoy = NULL;
	}

	if (m_pDI) {
		m_pDI->Release();
		m_pDI = NULL;
	}
}

//-----------------------------------------------------------------------
// Name: Init
// Desc: Initializes at least keyboard and mouse, otherwise returns error.
//		 If present joystick will also be initialized, but is not mandatory.
//-----------------------------------------------------------------------
HRESULT AweDXInput::Init(HWND hWnd, const RECT* prcCage)
{
	HRESULT hr;
	m_hWndMain = hWnd;

	// Create main DirectInput object
	if (FAILED(hr = DirectInput8Create(m_hDLL, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDI, NULL)))
	{
		return E_FAIL;
	}

	// Create all input device objects
	m_pKB = new AweKeyboard(m_pDI, hWnd);
	m_pMouse = new AweMouse(m_pDI, hWnd);
	m_pJoy = new AweJoystick(m_pDI, hWnd);


	// initialize all input device objects
	if (FAILED(m_pKB->Init()))
	{
		if (m_pKB) delete m_pKB;
		m_pKB = NULL;
		return E_FAIL;
	}

	if (FAILED(m_pMouse->Init()))
	{
		if (m_pMouse) delete m_pMouse;
		m_pMouse = NULL;
		return E_FAIL;
	}

	if (prcCage) m_pMouse->SetCage(*prcCage);

	if (FAILED(m_pJoy->Init()))
	{
		if (m_pJoy) delete m_pJoy;
		m_pJoy = NULL;
		// no need to fail 
	}

	m_bRunning = true;
	return S_OK;
}

//-----------------------------------------------------------------------
// Name: HasJoystick(
// Desc: Query if a Joystick is active and ready or not. Also returns
//		 Joytsicks name if any was found.
//-----------------------------------------------------------------------
bool AweDXInput::HasJoystick(char* pJoyName)
{
	if (m_pJoy)
	{
		if (pJoyName) m_pJoy->GetName(pJoyName);
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------
// Name: Update
// Desc: Update all input devices
//-----------------------------------------------------------------------
HRESULT AweDXInput::Update()
{
	HRESULT hr;

	if (!IsRunning()) return E_FAIL;

	if (m_pKB)
		if (FAILED(hr = m_pKB->Update()))
			return hr;


	if (m_pMouse)
		if (FAILED(hr = m_pMouse->Update()))
			return hr;

	if (m_pJoy)
		if (FAILED(hr = m_pJoy->Update()))
			return hr;

	return S_OK;
}

//-----------------------------------------------------------------------
// Name: GetPosition
// Desc: If mouse or joystick return current position.
//-----------------------------------------------------------------------
HRESULT AweDXInput::GetPosition(AWEINPUTDEV idType, POINT* pPt)
{
	if (idType == IDV_MOUSE)
	{
		m_pMouse->GetPosition(pPt);
		return S_OK;
	}
	else if (idType == IDV_JOYSTICK)
	{
		if (m_pJoy)
			m_pJoy->GetPosition(pPt);
		else
		{
			(*pPt).x = 0;
			(*pPt).y = 0;
		}
		return S_OK;
	}
	else return E_INVALIDARG;
}

//-----------------------------------------------------------------------
// Name: GetJoyDeflection
// Desc: If joystick, return current intensity on axes
//-----------------------------------------------------------------------
HRESULT AweDXInput::GetJoyDeflection(float* pflX, float* pflY, float* pfRX, float* pfRY)
{
	if (m_pJoy)
		return m_pJoy->GetJoyDeflection(pflX, pflY, pfRX, pfRY);
	else
	{
		(*pflX) = 0.0f;
		(*pflY) = 0.0f;
		(*pfRX) = 0.0f;
		(*pfRY) = 0.0f;
		return S_OK;
	}
}

//-----------------------------------------------------------------------
// Name: GetMouseDelta
// Desc: Return the change of mouse cursor since last call to Update().
//-----------------------------------------------------------------------
POINT AweDXInput::GetMouseDelta()
{
	return m_pMouse->GetMouseDelta();
}

//-----------------------------------------------------------------------
// Name: IsPressed
// Desc: Ask about button state.
//-----------------------------------------------------------------------
bool AweDXInput::IsPressed(AWEINPUTDEV idType, UINT nBtn)
{
	if (idType == IDV_MOUSE)
		return m_pMouse->IsPressed(nBtn);
	else if (idType == IDV_KEYBOARD)
		return m_pKB->IsPressed(nBtn);
	else if ((idType == IDV_JOYSTICK) && (m_pJoy))
		return m_pJoy->IsPressed(nBtn);
	else
		return false;
}

//-----------------------------------------------------------------------
// Name: IsReleased
// Desc: Ask about button/key state for mouse, joytsick or keyboard
//-----------------------------------------------------------------------
bool AweDXInput::IsReleased(AWEINPUTDEV idType, UINT nBtn)
{
	if (idType == IDV_MOUSE)
		return m_pMouse->IsReleased(nBtn);
	else if (idType == IDV_KEYBOARD)
		return m_pKB->IsReleased(nBtn);
	else if ((idType == IDV_JOYSTICK) && (m_pJoy))
		return m_pJoy->IsReleased(nBtn);
	else
		return false;
}

//-----------------------------------------------------------
// AweDIDevice class implementation                          
//-----------------------------------------------------------
//-----------------------------------------------------------------------
// Name: Create
// Desc: Set basic attributes to pointers
//-----------------------------------------------------------------------
void AweDIDevice::Create(LPDIRECTINPUT8 pDI, HWND hWnd)
{
	m_hWnd = hWnd;
	m_pDI = pDI;
	m_pDevice = NULL;
}

//-----------------------------------------------------------------------
// Name: Release
// Desc: Release the object.
//-----------------------------------------------------------------------
void AweDIDevice::Release(void)
{
	if (m_pDevice)
	{
		m_pDevice->Unacquire();
		m_pDevice->Release();
		m_pDevice = NULL;
	}
}

//-----------------------------------------------------------------------
// Name: CrankUp
// Desc: Call all stuff DirectInput needs to initialize an input device.
//-----------------------------------------------------------------------
HRESULT AweDIDevice::CrankUp(REFGUID rguid, LPCDIDATAFORMAT pdf)
{
	DWORD dwFlags = DISCL_FOREGROUND | DISCL_NONEXCLUSIVE;

	// If device is already build destroy it
	if (m_pDevice)
	{
		m_pDevice->Unacquire();
		m_pDevice->Release();
		m_pDevice = NULL;
	}

	// 1. Step: create device
	if (FAILED(m_pDI->CreateDevice(rguid, &m_pDevice, NULL)))
	{
		return E_FAIL;
	}

	// Set the correct device data format
	if (FAILED(m_pDevice->SetDataFormat(pdf)))
	{
		return E_FAIL;
	}

	// Set the cooperation level with windows
	if (FAILED(m_pDevice->SetCooperativeLevel(m_hWnd, dwFlags)))
	{
		return E_FAIL;
	}

	return S_OK;
}

//-----------------------------------------------------------------------
// Name: GetData
// Desc: Get the state or data from the device object.
//-----------------------------------------------------------------------
HRESULT AweDIDevice::GetData(AWEINPUTDEV Type, void* pData, DWORD* pdwNum)
{
	HRESULT hr = E_FAIL;
	size_t size = 0;

	// Is this a mouse?
	if (Type == IDV_MOUSE)
	{
		size = sizeof(DIDEVICEOBJECTDATA);

		hr = m_pDevice->GetDeviceData(size, (DIDEVICEOBJECTDATA*)pData, pdwNum, 0);
	}
	else
	{
		if (Type == IDV_KEYBOARD) size = sizeof(char) * 256;
		else size = sizeof(DIJOYSTATE);
		hr = m_pDevice->GetDeviceState(size, pData);
	}

	if (FAILED(hr))
	{
		// If lost or not yet acquired then acquire it at all costs
		if ((hr == DIERR_NOTACQUIRED) || (hr == DIERR_INPUTLOST))
		{
			hr = m_pDevice->Acquire();

			while (hr == DIERR_INPUTLOST)
				hr = m_pDevice->Acquire();

			// If another application is using this input device
			// have to give up and try next frame
			if (hr == DIERR_OTHERAPPHASPRIO) return S_OK;

			// If got back device then try again to read data
			if (SUCCEEDED(hr))
			{
				if (Type == IDV_MOUSE)
					hr = m_pDevice->GetDeviceData(size, (DIDEVICEOBJECTDATA*)pData, pdwNum, 0);
				else
					hr = m_pDevice->GetDeviceState(size, pData);
			}
			if (FAILED(hr)) return E_FAIL;
		}
		else return E_FAIL;
	}
	return S_OK;
}