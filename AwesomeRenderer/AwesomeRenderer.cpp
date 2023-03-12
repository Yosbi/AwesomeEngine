//-----------------------------------------------------------------------
// AwesomeRenderer.cpp
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 11/03/2023
//-----------------------------------------------------------------------
// Desc: This file has the implementation of the static lib used to create,
//		get a pointer to, and release a render device, it can be for now 
//		just D3D

#include "AwesomeRenderer.h"

//----------------------------------------------------------------------
// Name: AwesomeRenderer
// Desc: Constructor
//----------------------------------------------------------------------
AwesomeRenderer::AwesomeRenderer(HINSTANCE hInst) :
	m_hInst(hInst),
	m_pDevice(nullptr),
	m_hDLL(nullptr)
{
}

//----------------------------------------------------------------------
// Name: ~AwesomeRenderer
// Desc: Destructor
//----------------------------------------------------------------------
AwesomeRenderer::~AwesomeRenderer()
{
	// Release all allocated resourses
	Release();
	// Release DLL
	if (m_hDLL) FreeLibrary(m_hDLL);
}

//----------------------------------------------------------------------
// Name: Release
// Desc: Release all allocated resourses sniffing for the method
//		ReleaseRenderDevice inside the DLL
//----------------------------------------------------------------------
void AwesomeRenderer::Release()
{
	// Declaration of a pointer to the method ReleaseRenderDevice
	RELEASERENDERDEVICE _ReleaseRenderDevice = 0;

	if (m_hDLL)
	{
		// Get the address of this method on the DLL
		if (!(_ReleaseRenderDevice = (RELEASERENDERDEVICE)GetProcAddress(m_hDLL, "ReleaseRenderDevice")))
			return;

		// Call the dll release function
		if (m_pDevice)
		{
			if (FAILED(_ReleaseRenderDevice(&m_pDevice)))
				m_pDevice = NULL;
		}
	}
}

//----------------------------------------------------------------------
// Name: CreateDevice
// Desc: Create the DLL object loading the appropiate DLL
//----------------------------------------------------------------------
HRESULT AwesomeRenderer::CreateDevice()
{
	//MessageBox( NULL, L"ENTROOOOOOOOOOOOOOOOOOOOOOO", L"ENTROOOOOOOOOOOOOOOOOOOOOOO", MB_OK | MB_ICONERROR );

	m_hDLL = LoadLibrary(L"AweD3D.dll");
	if (!m_hDLL)
	{
		MessageBox(NULL, L"Loading AWED3D.dll from lib failed.", L"AwesomeEngine - Error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}


	// Pointer to the method CreateRenderDevice nside the dll
	CREATERENDERDEVICE _CreateRenderDevice = NULL;

	// Get the pointer
	if (!(_CreateRenderDevice = (CREATERENDERDEVICE)GetProcAddress(m_hDLL, "CreateRenderDevice")))
		return E_FAIL;

	// Call the dll function
	if (FAILED(_CreateRenderDevice(m_hDLL, &m_pDevice)))
	{
		MessageBox(NULL, L"CreateRenderDevice() from lib failed", L"AwesomeEngine - Error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	// Sucess!
	return S_OK;
}