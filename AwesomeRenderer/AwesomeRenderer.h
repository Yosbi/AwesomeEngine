//-----------------------------------------------------------------------
// AwesomeRenderer.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 11/03/2023
//-----------------------------------------------------------------------
// Desc: This file has the definition of the static lib used to create,
//		get a pointer to, and release a render device, it can be for now 
//		just D3D
#ifndef AWESOME_RENDERER_H
#define AWESOME_RENDERER_H
#pragma once

#include "AwesomeRenderDevice.h"
#include <stdio.h>

//----------------------------------------------------------------------
// Name: AwesomeRenderer (class)
// Desc:The class of the static lib used to create, get a pointer, and
//		release an object of a renderer dll
//----------------------------------------------------------------------
class AwesomeRenderer
{
public:
	// Constructor and destructor
	AwesomeRenderer(HINSTANCE hInst);
	~AwesomeRenderer();

	// Public functions
	HRESULT				CreateDevice();
	LPAWERENDERDEVICE	GetDevice() { return m_pDevice; }
	HINSTANCE			GetModule() { return m_hDLL; }
	void				Release();

private:
	// Private variables
	LPAWERENDERDEVICE	m_pDevice;	// Pointer to the object of the DLL casted to the interface type (YRendererDevice)
	HINSTANCE			m_hInst;	// Instance of the app
	HMODULE				m_hDLL;		// Handle of the loaded DLL
};
typedef class AwesomeRenderer* LPAWERENDERER;

#endif // !AWESOME_RENDERER

