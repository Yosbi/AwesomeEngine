#pragma once
#ifndef MAIN_H
#define MAIN_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include "..\AwesomeRenderer\AwesomeRenderer.h"
// D E F I N E S ///////////////////////////////////////////////////


// P R O T O T Y P E S /////////////////////////////////////////////

LRESULT   WINAPI MsgProc(HWND, UINT, WPARAM, LPARAM);
HRESULT   ProgramStartup();
HRESULT   ProgramCleanup(void);
HRESULT   ProgramTick(void);
HRESULT   Render(int);

void Log(char* chString, ...);

#endif