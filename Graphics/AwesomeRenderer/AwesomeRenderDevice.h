//-----------------------------------------------------------------------
// AwesomeRenderDevice.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 11/03/2023
//-----------------------------------------------------------------------
// Desc: This file contain a set of abstract classes which provides an interface
//		from which render dlls will inherit. This secures availability
//		of the nesessary render functions
#ifndef AWESOMERENDERDEVICE_H
#define AWESOMERENDERDEVICE_H
#pragma once

#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#include <string>
#include <vector>
#include "AwesomeSkinManager.h"
#include "AwesomeVertexCacheManager.h"
#include "../../Math/AwesomeMath/AwesomeMath.h"


//----------------------------------------------------------------------
// Name: AwesomeRenderDevice (class)
// Desc: The class of the static lib used to create, get a pointer, and
//		 release an object of a renderer dll
//----------------------------------------------------------------------
class AwesomeRenderDevice
{
public:
	// Const and dest
	AwesomeRenderDevice() {};
	virtual ~AwesomeRenderDevice() {};

	//---------------------------
	// Initialize and release
	//---------------------------
	// Initialize the engine, should first display dialog to select
	// graphics adapter and mode , and then start the 3D API
	virtual HRESULT Init(HWND hWnd) = 0;

	// Is well initialized the engine? / is the engine running?
	virtual bool IsRunning() = 0;

	//---------------------------
	// Rendering stuff
	//---------------------------

	// Clear buffer and prepare for rendering
	virtual HRESULT BeginRendering(bool bClearPixel, bool bClearDepth, bool bClearStencil) = 0;

	// End rendering and flip pixel buffer to front
	virtual void	EndRendering() = 0;

	// Clear pixel, depth and stencil buffer
	virtual HRESULT Clear(bool bClearPixel, bool bClearDepth, bool bClearStencil) = 0;

	// Change background color
	virtual void	SetClearColor(float fRed, float fGreen, float fBlue) = 0;

	// Section Camera-projection:
	// View matrix form position, fix point, worldup
	virtual void SetViewMatrix(const AWEVector& vcPos, const AWEVector& vcPoint, const AWEVector& vcWorldUp) = 0;

	// Set World matrix
	virtual void SetWorldTransformMatrix(const AWEMatrix& mWorld) = 0;

	// Set fov and near and far planes
	virtual void setClippingPlanes(float near, float far) = 0;

	// Set FoV
	virtual void setFoV(float FoV) = 0;

	//---------------------------
	// Lightning stuff
	//---------------------------
	virtual void SetAmbientLight(AWESOMECOLOR color) = 0;
	/*virtual void SetDiffuseLight(AWESOMECOLOR color) = 0;
	virtual void SetSpecularLight(AWESOMECOLOR color) = 0;
	virtual void SetLightDirection(AWEVector vcDir) = 0;*/
	virtual HRESULT AddLight(AWELIGHT& pLight, UINT &nID) = 0;
	virtual HRESULT UpdateLight(UINT nID, AWELIGHT& pLight) = 0;
	
	//---------------------------
	// Skin Manager stuff
	//---------------------------
	// Skin manager
	virtual AwesomeSkinManager* GetSkinManager() = 0;

	//---------------------------
	// Vertex Cache Manager stuff
	//---------------------------
	// Get vertex cache
	virtual AwesomeVertexCacheManager* GetVertexManager() = 0;

	//---------------------------
	// Shader stuff
	//---------------------------
	virtual HRESULT CreatePipelineStateObject(std::wstring sVertexShader, std::wstring sPixelShader, bool bWireframe, UINT &nID) = 0;
	virtual HRESULT SetActivePipelineStateObject(UINT nID) = 0;

	//---------------------------
	// Time stuff
	//---------------------------
	virtual void UpdateDeltaTime(float fDTime) = 0;

};
typedef class AwesomeRenderDevice* LPAWERENDERDEVICE;


//----------------------------------------------------------------------
// Desc: This are methods to export from the dll and typedef pointers
//		to be able to do the exportation
//----------------------------------------------------------------------
extern "C"
{
	HRESULT CreateRenderDevice(HINSTANCE hDLL, LPAWERENDERDEVICE* pInterface);
	typedef HRESULT(*CREATERENDERDEVICE)(HINSTANCE hDLL, LPAWERENDERDEVICE* pInterface);

	HRESULT ReleaseRenderDevice(LPAWERENDERDEVICE* pInterface);
	typedef HRESULT(*RELEASERENDERDEVICE)(LPAWERENDERDEVICE* pInterface);
}


#endif // !AWESOMERENDERDEVICE_H


