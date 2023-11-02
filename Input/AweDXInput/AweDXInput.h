//-------------------------------------------------------------------------------
// AweDXInput.h
// Yosbi Alves
// yosbi@outlook.com
// Created on 20/04/2023
//-----------------------------------------------------------------------
//Desc: The implemetation of the imputdevice 
//--------------------------------------------------------------------------
#ifndef AWEDXINPUT_H
#define AWEDXINPUT_H
#pragma once

#define DIRECTINPUT_VERSION 0x0800

//-----------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------
#include "../AwesomeInput/AwesomeInputDevice.h"
#include <dinput.h>

//-----------------------------------------------------------------------
// Fordward class declarations
//--------------------------------------------------------------------------
class AweKeyboard;
class AweMouse;
class AweJoystick;

//-----------------------------------------------------------------------
// DLL entrypoint function
//--------------------------------------------------------------------------
BOOL WINAPI DllEntryPoint(HINSTANCE hDll, DWORD fdwReason, LPVOID lpvRserved);

//-----------------------------------------------------------------------
// Name: AweDXInput(Class)
// Desc: Class definition for a input device implementing the YInputDevice
//-----------------------------------------------------------------------
class AweDXInput : public AwesomeInputDevice
{
public:
    AweDXInput(HINSTANCE hDLL);
    ~AweDXInput();

    // initialize the engine stuff
    HRESULT Init(HWND, const RECT*);

    // Interface functions
    void    Release();
    bool    IsRunning() { return m_bRunning; }
    bool    HasJoystick(char* pJoyName);
    HRESULT Update();

    // Query input data and states
    HRESULT GetPosition(AWEINPUTDEV idType, POINT* pPt);
    HRESULT GetJoyDeflection(float*, float*, float*, float*);
    POINT   GetMouseDelta();
    bool    IsPressed(AWEINPUTDEV idType, UINT nBtn);
    bool    IsReleased(AWEINPUTDEV idType, UINT nBtn);


private:

    LPDIRECTINPUT8  m_pDI;
    AweKeyboard* m_pKB;
    AweMouse* m_pMouse;
    AweJoystick* m_pJoy;

    HWND       m_hWndMain;          // Application main window
    HINSTANCE  m_hDLL;              // Dll module handle
    bool       m_bRunning;          // After succesful initialization
};

//-----------------------------------------------------------------------
// Name: AweDIDevice(Class)
// Desc: Joystick, mause and kb general class
//-----------------------------------------------------------------------
class AweDIDevice
{
public:
    AweDIDevice() { }
    virtual  ~AweDIDevice() { }

    // base class methods
    virtual void    Create(LPDIRECTINPUT8, HWND);
    virtual void    Release();
    virtual HRESULT CrankUp(REFGUID rguid, LPCDIDATAFORMAT lpdf);

    // Accessor-Methods
    virtual void    GetPosition(POINT* pPoint) { (*pPoint).x = m_lX; (*pPoint).y = m_lY; }

    // virtual methods for heirs
    virtual HRESULT Init() = 0;
    virtual HRESULT Update() = 0;

protected:
    virtual HRESULT GetData(AWEINPUTDEV Type, void* pData, DWORD* dwNum);

    LPDIRECTINPUTDEVICE8  m_pDevice;
    LPDIRECTINPUT8        m_pDI;
    HWND                  m_hWnd;
    long                  m_lX;
    long                  m_lY;
    long                  m_rX;
    long                  m_rY;
};

//-----------------------------------------------------------------------
// Name: AweKeyboard(Class)
// Desc: Keyboard Device
//-----------------------------------------------------------------------
class AweKeyboard : public AweDIDevice
{
public:
    AweKeyboard(LPDIRECTINPUT8, HWND);
    virtual ~AweKeyboard();

    HRESULT Init();
    HRESULT Update();

    bool    IsPressed(UINT nID);
    bool    IsReleased(UINT nID);

private:
    char  m_Keys[256];
    char  m_KeysOld[256];
};

//-----------------------------------------------------------------------
// Name: AweMouse(Class)
// Desc: Mouse Device
//-----------------------------------------------------------------------
class AweMouse : public AweDIDevice
{
public:
    AweMouse(LPDIRECTINPUT8, HWND);
    virtual ~AweMouse();

    HRESULT Init();
    HRESULT Update();

    void  SetCage(RECT rcCage) { m_rcCage = rcCage; }
    POINT GetMouseDelta() { return m_Delta; }

    bool IsPressed(UINT nBtn)
    {
        if (nBtn < 3) return m_bPressed[nBtn]; return false;
    }
    bool IsReleased(UINT nBtn)
    {
        if (nBtn < 3) return m_bReleased[nBtn]; return false;
    }

private:
    HANDLE     m_hEvent;
    RECT       m_rcCage;
    bool       m_bPressed[3];
    bool       m_bReleased[3];
    POINT      m_Delta;
};

//-----------------------------------------------------------------------
// Name: AweJoystick(Class)
// Desc: Joystick / Gamepad Device
//-----------------------------------------------------------------------
class AweJoystick : public AweDIDevice
{
public:
    AweJoystick(LPDIRECTINPUT8, HWND);
    virtual ~AweJoystick();

    HRESULT Init();
    HRESULT Update();

    bool JoystickFound() { return m_bJoyFound; }
    BOOL EnumJoyCallback(const DIDEVICEINSTANCE* pInst);

    void GetName(char* pJoyName) { memcpy(pJoyName, m_Name, sizeof(char) * 256); }

    bool IsPressed(UINT nBtn) { if (nBtn < m_dwNumBtns) return m_bPressed[nBtn]; return false; }

    bool IsReleased(UINT nBtn) { if (nBtn < m_dwNumBtns) return m_bReleased[nBtn]; return false; }

    HRESULT GetJoyDeflection(float* pfX, float* pfY, float* pfRX, float* pfRY);

private:
    GUID       m_guid;
    char       m_Name[256];
    bool       m_bJoyFound;
    bool       m_bPressed[24];
    bool       m_bReleased[24];
    DWORD      m_dwNumBtns;
};

#endif
