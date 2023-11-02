#pragma once
#ifndef MAIN_H
#define MAIN_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <vector>
#include <iostream>
#include "..\..\Graphics\AwesomeRenderer\AwesomeRenderer.h"
#include "..\..\Math\AwesomeMath\AwesomeMath.h"
#include "..\..\Input\AwesomeInput\AwesomeInput.h"
#include "..\..\Utils\AwesomeGeneral\AwesomeGeneral.h"
// D E F I N E S ///////////////////////////////////////////////////


// P R O T O T Y P E S /////////////////////////////////////////////

LRESULT   WINAPI MsgProc(HWND, UINT, WPARAM, LPARAM);
HRESULT   RendererStartup();
HRESULT	  LoadAssets();
HRESULT   ProgramCleanup(void);
HRESULT   ProgramTick(void);
void	  initCamera();
HRESULT   InputStartup();
void      updateInput();
void	  updateCamera(AWEVector deltaPos);
void	  updateFPS();
void	  GenGrid(int numVertRows, int numVertCols, float dx, float dz, const AWEVector& center);


#endif