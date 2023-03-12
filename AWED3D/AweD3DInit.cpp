//-----------------------------------------------------------------------
// AweD3DInit.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 11/03/2023
//-----------------------------------------------------------------------
//Desc: In this file it the implementation of the code to initialize
//		the engine and of the methods to export from the dll
//--------------------------------------------------------------------------

//-----------------------------------------------------------------------
// Include
//--------------------------------------------------------------------------
#include "AweD3D.h"

//-----------------------------------------------------------------------------
// Name: DLLEntryPoint
// Desc: Is similar to WinMain()/main() functions, is used to initialize
//		the DLL
//-----------------------------------------------------------------------------
BOOL WINAPI DllEntryPoint(HINSTANCE hDll, DWORD fdwReason, LPVOID lpcReserved)
{
	switch (fdwReason)
	{
		//Called when we attach to the dll
	case DLL_PROCESS_ATTACH:
		/* dll init/setup stuff */
		break;
	case DLL_PROCESS_DETACH:
		/* dll shutdow/release stuff */
		break;
	default:
		break;
	}
	return TRUE;
}

//-----------------------------------------------------------------------------
// Name: CreateRenderDevice
// Desc: Exported created function, creates a new YRenderDevice object
//-----------------------------------------------------------------------------
HRESULT CreateRenderDevice(HINSTANCE hDLL, AwesomeRenderDevice** pDevice)
{
	if (!*pDevice)
	{
		*pDevice = new AweD3D(hDLL);
		return S_OK;
	}
	return E_FAIL;
}

//-----------------------------------------------------------------------------
// Name: ReleaseRenderDevice
// Desc: Exported release function: Releases the given YRenderDevice object
//-----------------------------------------------------------------------------
HRESULT ReleaseRenderDevice(AwesomeRenderDevice** pDevice)
{
	if (!*pDevice) return E_FAIL;

	delete* pDevice;
	*pDevice = NULL;

	return S_OK;
}

//-----------------------------------------------------------------------------
// AweD3D class implementations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name: AweD3D
// Desc: Constructor
//-----------------------------------------------------------------------------
AweD3D::AweD3D(HINSTANCE hDll) :
	m_hDLL(hDll), 
	m_hWnd(nullptr),
	m_nClientWidth(1280),
	m_nClientHeight(720),
	m_bWindowed(true),
	m_bRunning(false),
	m_bIsSceneRunning(false),
	m_bStencil(false),
	m_d3d12Device(nullptr),
	m_dxgiFactory(nullptr),
	m_SwapChain(nullptr),
	m_ClearColor({ 0.0f, 0.0f, 0.0f, 1.0f }),
	m_pCommandQueue(nullptr)
{
	// Update the viewport transform to cover the client area.
	m_ScreenViewport.TopLeftX = 0;
	m_ScreenViewport.TopLeftY = 0;
	m_ScreenViewport.Width = static_cast<float>(m_nClientWidth);
	m_ScreenViewport.Height = static_cast<float>(m_nClientHeight);
	m_ScreenViewport.MinDepth = 0.0f;
	m_ScreenViewport.MaxDepth = 1.0f;

	// The sissor is the entire view port
	m_ScissorRect = { 0, 0, m_nClientWidth, m_nClientHeight };
}

//-----------------------------------------------------------------------------
// Name: ~AweD3D
// Desc: Destructor
//-----------------------------------------------------------------------------
AweD3D::~AweD3D()
{
	// Clear all values
	this->Release();
}

//-----------------------------------------------------------------------------
// Name: Release
// Desc: Release all the Direct3D COM stuff
//-----------------------------------------------------------------------------
void AweD3D::Release()
{
	m_pCommandQueue->Flush();
	delete m_pCommandQueue;
	m_pCommandQueue = nullptr;
}

//-----------------------------------------------------------------------------
// Name: Init
// Desc: Initialize  the idea here will be to use a dialogbox to select 
// the device and the formats, for now we will init this as vaunilla as possible
//-----------------------------------------------------------------------------
HRESULT AweD3D::Init(HWND hWnd)
{
	m_hWnd = hWnd;


	if (!InitDirect3D())
		return E_FAIL;

	// Do the initial resize code.
	OnResize();

	return S_OK;
	
}

//-----------------------------------------------------------------------------
// Name: InitDirect3D
// Desc: Initialize the direct3D objects
//-----------------------------------------------------------------------------
bool AweD3D::InitDirect3D()
{
#if defined(DEBUG) || defined(_DEBUG) 
	// Enable the D3D12 debug layer.
	{
		ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
#endif

	// Creating factory
	ThrowIfFailed(CreateDXGIFactory(IID_PPV_ARGS(&m_dxgiFactory)));

	// Try to create hardware device.
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,             // default adapter
		D3D_FEATURE_LEVEL_12_2,
		IID_PPV_ARGS(&m_d3d12Device));

	// Fallback to WARP device.
	if (FAILED(hardwareResult))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_12_0,
			IID_PPV_ARGS(&m_d3d12Device)));
	}



	// Getting sizes of resource descriptors(views) as these are hardware deppendant
	m_RtvDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_DsvDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_CbvSrvUavDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Checking for 4X MSAA support for the backbuffer format
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = m_BackBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(m_d3d12Device->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels)));

	m_n4xMsaaQuality = msQualityLevels.NumQualityLevels;
	assert(m_n4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

	// Creating command queue 
	m_pCommandQueue = new AweD3DCommandQueue(m_d3d12Device, D3D12_COMMAND_LIST_TYPE_DIRECT);

	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();

	return true;
}

void AweD3D::CreateSwapChain()
{
	// Release the previous swapchain we will be recreating.
	m_SwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = m_nClientWidth;
	sd.BufferDesc.Height = m_nClientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = m_BackBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = m_b4xMsaaState ? 4 : 1;
	sd.SampleDesc.Quality = m_b4xMsaaState ? (m_n4xMsaaQuality - 1) : 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = sm_nSwapChainBufferCount;
	sd.OutputWindow = m_hWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Note: Swap chain uses queue to perform flush.
	ThrowIfFailed(m_dxgiFactory->CreateSwapChain(
		m_pCommandQueue->GetD3D12CommandQueue().Get(),
		&sd,
		m_SwapChain.GetAddressOf()));

	// Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
	// will be handled manually.
	ThrowIfFailed(m_dxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER));
}

void AweD3D::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = sm_nSwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_d3d12Device->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(m_RtvHeap.GetAddressOf())));


	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_d3d12Device->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(m_DsvHeap.GetAddressOf())));
}