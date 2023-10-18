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
#include "../AwesomeGeneral/AwesomeTimer.h"
#include <initguid.h>
#include <wrl/client.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <dxgi1_5.h>
#include <DirectXColors.h>
#include <directxmath.h>
#include <d3dcompiler.h>
#include <string>
#include <comdef.h>
#include "d3dx12.h"
#include "AweD3DException.h"
#include "AweD3DCommandQueue.h"
#include "AweD3DSkinManager.h"
#include "AweD3DVertexCache.h"
#include "AweUtil.h"
#include "Awe.h"
#include <vector>



//----------------------------------------------------------------------
// Linking necessary d3d12 libraries
//----------------------------------------------------------------------
#pragma comment(lib, "AwesomeMath.lib")
#pragma comment(lib, "AwesomeGeneral.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3DCompiler.lib")

//using Microsoft::WRL::ComPtr;	// TODO: Avoid this
//using namespace DirectX;		// TODO: Avoid this

//----------------------------------------------------------------------
// Name: DLLEntryPoint
// Desc: Is similar to WinMain()/main() functions, is used to initialize
//		the DLL
//----------------------------------------------------------------------
BOOL WINAPI	DllEntryPoint(HINSTANCE hDll, DWORD fdwReason, LPVOID lpvReserved);

#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
#endif

class AweD3DVertexCacheManager;
class AweD3DSkinManager;

class AweD3D : public AwesomeRenderDevice
{
	friend AweD3DVertexCacheManager;
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
	HRESULT BeginRendering(bool bClearPixel, bool bClearDepth, bool bClearStencil);// Clear buffer and prepare for rendering
	void	EndRendering();																		// End rendering and flip pixel buffer to front
	HRESULT Clear(bool bClearPixel, bool bClearDepth, bool bClearStencil);						// Clear pixel, depth and stencil buffer
	void	SetClearColor(float fRed, float fGreen, float fBlue);								// Change background color
	bool	IsSceneRunning();
	D3D12_VIEWPORT* GetScreenViewPort();
	D3D12_RECT*	   GetSissorRect();

	//---------------------------
	// Mesh things
	//---------------------------
	//UINT LoadMesh(std::wstring sFileName);
	//UINT LoadMesh(std::wstring sFileName, float red, float green, float blue, float alpha);
	//void LoadMeshToGPU(unsigned int meshIndex);
	//void RenderMesh(unsigned int meshIndex);

	//---------------------------
	// Projection things
	//---------------------------
	void setClippingPlanes(float near, float far);
	void setFoV(float FoV);
	void SetViewMatrix(const AWEVector& vcPos, const AWEVector& vcPoint, const AWEVector& vcWorldUp);
	void SetWorldTransformMatrix(const AWEMatrix& mWorld);

	//---------------------------
	// Lightning stuff
	//---------------------------
	void SetAmbientLight(AWESOMECOLOR color);
	/*void SetDiffuseLight(AWESOMECOLOR color);
	void SetSpecularLight(AWESOMECOLOR color);
	void SetLightDirection(AWEVector vcDir);*/
	HRESULT AddLight(AWELIGHT& pLight, UINT& nID);
	HRESULT UpdateLight(UINT nID, AWELIGHT& pLight);

	//---------------------------
	// Skin manager
	//---------------------------
	AwesomeSkinManager* GetSkinManager();
	UINT	GetActiveSkinID() { return m_nActiveSkin; }
	void	SetActiveSkinID(UINT nID) { m_nActiveSkin = nID; }

	//---------------------------
	// Vertex Cache Manager stuff
	//---------------------------
	AwesomeVertexCacheManager* GetVertexManager();

	//---------------------------
	// Shader stuff
	//---------------------------
	HRESULT CreatePipelineStateObject(std::wstring sVertexShader, std::wstring sPixelShader, bool bWireframe, UINT& nID);
	HRESULT SetActivePipelineStateObject(UINT nID);

	//---------------------------
	// time stuff
	//---------------------------
	void UpdateDeltaTime(float fDTime);


private:
	// Private functions
	virtual void Release(); // Release memory and objects created


	HRESULT Go();																		// Start API with values from dialog box
	void	Log(std::wstring, ...);														// Write to log file

	bool EnableDebugLayer();
	void InitFactory();
	int  SelectAdapter();
	void CreateDevice();
	void CreateCommandQueue();
	void CreateDefaultRootSignature();
	void CreateDefaultPipelineStateObject();
	void CheckTearingSupport();
	void InitDescriptorSizes();
	void InitSkinManager();
	void InitVertexManager();
	bool InitDirect3D();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void ResizeSwapChain();
	void UpdateRenderTargetView();
	void UpdateDepthStencilView();
	void InitUploadBuffers();
	
	HRESULT CreatePipelineStateObject(Microsoft::WRL::ComPtr<ID3DBlob> vertexShader, Microsoft::WRL::ComPtr<ID3DBlob> pixelShader, bool bWireframe);
	ID3D12PipelineState* GetActivePipelineStateObject();

	
	
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, 
		uint32_t numDescriptors);
		

	void OnResize();

	D3D12_CPU_DESCRIPTOR_HANDLE getDepthStencilView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE getCurrentBackBufferView() const;
	ID3D12Resource* getCurrentBackBuffer()const;


	// Camera and projection things
	void SetProjMatrix();
	void UpdateMainPassVariablesCB();
	void UpdatePerObjectVariablesCB();


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
	float					m_fFoV;
	float					m_fNear;
	float					m_fFar;
	bool					m_bIsSceneRunning;
	bool					m_bSettedPassVariablesCB;
	UINT					m_nActiveSkin;


	//AWEMatrix m_WorldMatrix;
	AWEMatrix m_ViewMatrix;
	AWEMatrix m_ProjectionMatrix;

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

	// Buffer Descriptor heaps
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DsvHeap;

	// Viewport and sissor
	D3D12_VIEWPORT m_ScreenViewport;
	D3D12_RECT m_ScissorRect;

	// CommandQueue
	std::shared_ptr<AweD3DCommandQueue> m_pCommandQueue;
	float m_ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	bool m_bStencil;
	bool m_bTearingSupported;
	bool m_bVSync;

	// Default root signature and pipeline state and shader
	// TODO: do this dinamically later
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
	//Microsoft::WRL::ComPtr<ID3DBlob> m_vertexShader;
	//Microsoft::WRL::ComPtr<ID3DBlob> m_pixelShader;


	// Pipeline state
	std::vector< Microsoft::WRL::ComPtr<ID3D12PipelineState>> m_vcPipelineState;
	UINT m_nPipelineStateIndex;


	// Modules of the engine
	AweD3DSkinManager m_SkinManager;

	// Meshes list
	AweD3DVertexCacheManager m_VertexManager;

	// Per pass variables sent to the shader
	std::unique_ptr<AweD3DUploadBuffer<AWEPASSVARIABLES>> m_CbUploadPassVariables = nullptr; // Upload constant buffer
	AWEPASSVARIABLES m_PassVariables;														 // The actual variable
	//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CbvHeapPassVariables = nullptr;			 // The heap

	// Per object variables sent to the shader
	std::unique_ptr<AweD3DUploadBuffer<AWEPEROBJECTVARIABLES>> m_CbUploadPerObjectVariables = nullptr;	// Upload constant buffer
	AWEPEROBJECTVARIABLES m_PerObjectVariables;															// The actual variable
	//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CbvHeapPerObjectVariables = nullptr;					// The heap

	std::vector <IDXGIAdapter*> m_vcAdapters;
};

#endif // !AWED3D_H

