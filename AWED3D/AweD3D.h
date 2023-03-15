//-----------------------------------------------------------------------
// AweD3D.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 11/03/2023
//-----------------------------------------------------------------------
// Desc: This file contains all related functions, methods an data structs
//		to manipulate, initialize, shuts down among other things the Direct3D

#ifndef AWED3D_H
#define AWED3D_H
#pragma once

//----------------------------------------------------------------------
// Definitions
//----------------------------------------------------------------------
#define MAX_3DHWND 8	// Defines max 8 child windows

//----------------------------------------------------------------------
// includes
//----------------------------------------------------------------------
#include "../AwesomeRenderer/AwesomeRenderDevice.h"
#include "d3dx12.h"
#include <dxgi.h>
#include <dxgi1_4.h>
#include <dxgi1_5.h>
#include <DirectXColors.h>
#include <directxmath.h>
#include "AweD3DException.h"
#include "AweD3DCommandQueue.h"


//----------------------------------------------------------------------
// Linking necessary d3d12 libraries
//----------------------------------------------------------------------
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3DCompiler.lib")

using Microsoft::WRL::ComPtr;	// TODO: Avoid this
using namespace DirectX;		// TODO: Avoid this

//----------------------------------------------------------------------
// Name: DLLEntryPoint
// Desc: Is similar to WinMain()/main() functions, is used to initialize
//		the DLL
//----------------------------------------------------------------------
BOOL WINAPI	DllEntryPoint(HINSTANCE hDll, DWORD fdwReason, LPVOID lpvReserved);

#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
#endif


class AweD3D : public AwesomeRenderDevice
{
public:
	// Const and dest
	AweD3D(HINSTANCE hDll); 
	~AweD3D();

	//---------------------------
	// Initialize and release
	//---------------------------
	virtual HRESULT Init(HWND hWnd);
	
	
	virtual bool IsRunning();

	//---------------------------
	// Rendering stuff
	//---------------------------
	HRESULT BeginRendering(bool bClearPixel, bool bClearDepth, bool bClearStencil);		// Clear buffer and prepare for rendering
	void	EndRendering();																// End rendering and flip pixel buffer to front
	HRESULT Clear(bool bClearPixel, bool bClearDepth, bool bClearStencil);				// Clear pixel, depth and stencil buffer
	void	SetClearColor(float fRed, float fGreen, float fBlue);						// Change background color

private:
	// Private functions
	virtual void Release(); // Release memory and objects created


	HRESULT Go();																		// Start API with values from dialog box
	void	Log(std::wstring, ...);														// Write to log file

	

	bool EnableDebugLayer();
	void InitFactory();
	void CreateDevice();
	void CreateCommandQueue();
	void CheckTearingSupport();
	void InitDescriptorSizes();
	bool InitDirect3D();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void ResizeSwapChain();
	void UpdateRenderTargetView();
	void UpdateDepthStencilView();
	ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors);
		

	void OnResize();

	D3D12_CPU_DESCRIPTOR_HANDLE getDepthStencilView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE getCurrentBackBufferView() const;
	ID3D12Resource* getCurrentBackBuffer()const;

private:

	//---------------------------
	// Interface parameters
	//---------------------------
	HWND					m_hWnd;				// 3D render window handle
	HINSTANCE				m_hDLL;				// DLL module handle
	int						m_nClientWidth;		// Screen width
	int						m_nClientHeight;	// Screen Height
	bool					m_bWindowed;		// Windowed mode?
	bool					m_bRunning;			// After succesfull 


	Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
	Microsoft::WRL::ComPtr<ID3D12Device2> m_d3d12Device;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> m_SwapChain;

	// Buffer descriptors sizes
	UINT m_RtvDescriptorSize = 0;		// Render target 
	UINT m_DsvDescriptorSize = 0;		// Deep Stencil 
	UINT m_CbvSrvUavDescriptorSize = 0;	// Constant buffer

	// Buffers formats
	DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM; // Backbuffer format
	DXGI_FORMAT m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT; // DeepStencil 

	// Swap chain stuff and actual buffers refs
	static const int sm_nSwapChainBufferCount = 2;
	int m_nCurrBackBuffer = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_SwapChainBuffer[sm_nSwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;

	// Buffer Descriptor heaps
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DsvHeap;

	// Viewport and sissor
	D3D12_VIEWPORT m_ScreenViewport;
	D3D12_RECT m_ScissorRect;

	// CommandQueue
	AweD3DCommandQueue* m_pCommandQueue;
	float m_ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	bool m_bIsSceneRunning;
	bool m_bStencil;
	bool m_bTearingSupported;
	bool m_bVSync;

};

#endif // !AWED3D_H

