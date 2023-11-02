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
#include "Waves.h"
// D E F I N E S ///////////////////////////////////////////////////


// P R O T O T Y P E S /////////////////////////////////////////////

static float RandF()
{
	return (float)(rand()) / (float)RAND_MAX;
}
static int Rand(int a, int b)
{
    return a + rand() % ((b - a) + 1);
}
static float RandF(float a, float b)
{
	return a + RandF() * (b - a);
}

LRESULT   WINAPI MsgProc(HWND, UINT, WPARAM, LPARAM);
HRESULT   RendererStartup();
HRESULT	  LoadAssets();
HRESULT   ProgramCleanup(void);
HRESULT   ProgramTick(void);
void	  initCamera();
HRESULT   InputStartup();
void      updateInput();
void	  updateCamera(float deltaT);
void	  updateFPS();
float	  GetHillsHeight(float x, float z);
AWEVector GetHillsNormal(float x, float z);
void	  UpdateWaves();
void	  initScene();

#endif