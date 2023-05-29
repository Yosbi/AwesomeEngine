#pragma once
#ifndef MAIN_H
#define MAIN_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <vector>
#include <iostream>
#include "..\..\AwesomeRenderer\AwesomeRenderer.h"
#include "..\..\AwesomeMath\AwesomeMath.h"
#include "..\..\AwesomeInput\AwesomeInput.h"
#include "..\..\AwesomeGeneral\AwesomeGeneral.h"
#include "Particles.h"
#include <ppl.h>
#include <algorithm>
#include <vector>
#include <cassert>

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
void	  initScene();
void	  updateLight();
void	  Render();


#endif