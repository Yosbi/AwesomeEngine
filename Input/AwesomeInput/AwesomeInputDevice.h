//-------------------------------------------------------------------------------
// AwesomeInputDevice.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 20/04/2023
//-----------------------------------------------------------------------
//Desc: The interface to get the events of the keyboard, mause and any joystick
//		without delay
//--------------------------------------------------------------------------

#ifndef AWESOMEINPUTINTERFACE_H
#define AWESOMEINPUTINTERFACE_H
#pragma once

//-----------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------
#include <Windows.h>

//----------------------------------------------------------------------
// Name: AWEINPUTDEV
// Desc: Definition of the devices input types
//----------------------------------------------------------------------
typedef enum AWEINPUTDEV_TYPE
{
	IDV_KEYBOARD,
	IDV_JOYSTICK,
	IDV_MOUSE
} AWEINPUTDEV;

//-----------------------------------------------------------------------
// Name: AwesomeInput(Class)
// Desc: Class used to get a object to the input module
//-----------------------------------------------------------------------
class AwesomeInputDevice
{
public:
	AwesomeInputDevice(void) {};
	virtual ~AwesomeInputDevice(void) {};

	// Ini the module
	virtual HRESULT Init(HWND, const RECT*) = 0;

	// Release API specific stuff
	virtual void	Release(void) = 0;

	// Is initialized?
	virtual bool	IsRunning(void) = 0;

	// Joystick available?
	virtual bool	HasJoystick(char* pJoyName) = 0;

	// Poll input devices
	virtual HRESULT	Update(void) = 0;

	// Will work for mouse and joystick only
	virtual HRESULT GetPosition(AWEINPUTDEV, POINT*) = 0;

	// For joysticks only (intensity range from -1 to +1)
	virtual HRESULT GetJoyDeflection(float* pfLX, float* pfLY, float* pfRX, float* pfRY) = 0;

	// Get change in mouse position
	virtual POINT	GetMouseDelta(void) = 0;

	// Will work for keyboard, mouse and joystick
	virtual bool	IsPressed(AWEINPUTDEV, UINT) = 0;

	// Will work for keyboard, mouse and joystick
	virtual bool	IsReleased(AWEINPUTDEV, UINT) = 0;

};
typedef class AwesomeInputDevice* LPAWEINPUTDEVICE;


extern "C"
{
	HRESULT CreateInputDevice(HINSTANCE hDLL, AwesomeInputDevice** pInterface);
	typedef HRESULT(*CREATEINPUTDEVICE)(HINSTANCE hDLL, AwesomeInputDevice** pInterface);

	HRESULT ReleaseInputDevice(AwesomeInputDevice** pInterface);
	typedef HRESULT(*RELEASEINPUTDEVICE)(AwesomeInputDevice** pInterface);
}

//-----------------------------------------------------------------------
// The next definitions is the codes of the buttons that this device uses
//-----------------------------------------------------------------------
// keyboard scan codes from dinput.h
#define AWEVK_ESCAPE          0x01
#define AWEVK_TAB             0x0F
#define AWEVK_SPACE           0x39
#define AWEVK_RETURN          0x1C
#define AWEVK_BACK            0x0E
#define AWEVK_CAPITAL         0x3A

#define AWEVK_MINUS           0x0C
#define AWEVK_EQUALS          0x0D
#define AWEVK_LBRACKET        0x1A
#define AWEVK_RBRACKET        0x1B
#define AWEVK_SEMICOLON       0x27
#define AWEVK_APOSTROPHE      0x28
#define AWEVK_GRAVE           0x29
#define AWEVK_BACKSLASH       0x2B
#define AWEVK_COMMA           0x33
#define AWEVK_PERIOD          0x34
#define AWEVK_SLASH           0x35

// CHARACTER KEYS
#define AWEVK_A               0x1E
#define AWEVK_S               0x1F
#define AWEVK_D               0x20
#define AWEVK_F               0x21
#define AWEVK_G               0x22
#define AWEVK_H               0x23
#define AWEVK_J               0x24
#define AWEVK_K               0x25
#define AWEVK_L               0x26
#define AWEVK_Z               0x2C
#define AWEVK_X               0x2D
#define AWEVK_C               0x2E
#define AWEVK_V               0x2F
#define AWEVK_B               0x30
#define AWEVK_N               0x31
#define AWEVK_M               0x32
#define AWEVK_Q               0x10
#define AWEVK_W               0x11
#define AWEVK_E               0x12
#define AWEVK_R               0x13
#define AWEVK_T               0x14
#define AWEVK_AWE             0x15
#define AWEVK_U               0x16
#define AWEVK_I               0x17
#define AWEVK_O               0x18
#define AWEVK_P               0x19

// MAIN NUM KEYS
#define AWEVK_1               0x02
#define AWEVK_2               0x03
#define AWEVK_3               0x04
#define AWEVK_4               0x05
#define AWEVK_5               0x06
#define AWEVK_6               0x07
#define AWEVK_7               0x08
#define AWEVK_8               0x09
#define AWEVK_9               0x0A
#define AWEVK_0               0x0B

// FUNCTION KEYS
#define AWEVK_F1              0x3B
#define AWEVK_F2              0x3C
#define AWEVK_F3              0x3D
#define AWEVK_F4              0x3E
#define AWEVK_F5              0x3F
#define AWEVK_F6              0x40
#define AWEVK_F7              0x41
#define AWEVK_F8              0x42
#define AWEVK_F9              0x43
#define AWEVK_F10             0x44
#define AWEVK_F11             0x57
#define AWEVK_F12             0x58

// ON NUMPAD
#define AWEVK_NPPLUS          0x4E
#define AWEVK_NPMINUS         0x4A
#define AWEVK_NPDECIMAL       0x53
#define AWEVK_NPCOMMA         0xB3
#define AWEVK_NPDIVIDE        0xB5
#define AWEVK_NPMULTIPLAWE    0x37
#define AWEVK_NPENTER         0x9C
#define AWEVK_NUMLOCK         0x45
#define AWEVK_NP1             0x4F
#define AWEVK_NP2             0x50
#define AWEVK_NP3             0x51
#define AWEVK_NP4             0x4B
#define AWEVK_NP5             0x4C
#define AWEVK_NP6             0x4D
#define AWEVK_NP7             0x47
#define AWEVK_NP8             0x48
#define AWEVK_NP9             0x49
#define AWEVK_NP0             0x52

// CTRL, ALT, SHFT and WINDWS
#define AWEVK_RSHIFT          0x36
#define AWEVK_LSHIFT          0x2A
#define AWEVK_RCTRL           0x9D
#define AWEVK_LCTRL           0x1D
#define AWEVK_RALT            0xB8
#define AWEVK_LALT            0x38
#define AWEVK_LWIN            0xDB
#define AWEVK_RWIN            0xDC

// ON ARROW KEAWEPAD
#define AWEVK_UP              0xC8
#define AWEVK_DOWN            0xD0
#define AWEVK_LEFT            0xCB
#define AWEVK_RIGHT           0xCD
#define AWEVK_INSERT          0xD2
#define AWEVK_DELETE          0xD3
#define AWEVK_HOME            0xC7
#define AWEVK_END             0xCF
#define AWEVK_PGDOWN          0xD1
#define AWEVK_PGUP            0xC9

#define AWEVK_PAUSE           0xC5    /* Pause */
#define AWEVK_SCROLL          0x46    /* Scroll Lock */


#endif
