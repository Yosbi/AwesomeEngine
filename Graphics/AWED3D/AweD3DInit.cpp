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
	m_bSettedPassVariablesCB(false),
	m_bStencil(false),
	m_d3d12Device(nullptr),
	m_dxgiFactory(nullptr),
	m_SwapChain(nullptr),
	m_pCommandQueue(nullptr),
	m_bVSync(true),
	m_bTearingSupported(false),
	m_fFoV(45.0f),
	m_fNear(0.1f),
	m_fFar(100.0f),
	m_nActiveSkin(MAX_ID),
	m_nPipelineStateIndex(0),
	m_ScissorRect({ 0, 0, m_nClientWidth, m_nClientHeight }),
	m_ScreenViewport({/*TopLeftX*/ 0, /*TopLeftY*/ 0, 
		/*Width*/ static_cast<float>(m_nClientWidth), 
		/*Height*/ static_cast<float>(m_nClientHeight) , 
		/*MinDepth*/ 0.0f, /*MaxDepth*/ 1.0f})
{
	
	SetProjMatrix();

	//SetViewMatrix(AWEVector(0, 0, -30), AWEVector(0, 0, 0), AWEVector(0, 1, 0));
	
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
}


//-----------------------------------------------------------------------------
// Name: EnableDebugLayer
// Desc: if DEBUG flag is defined, it allows the to log all the possible errors
// produced by the DX12 objects
//-----------------------------------------------------------------------------
bool AweD3D::EnableDebugLayer()
{
#if defined(DEBUG) || defined(_DEBUG) 
	// Enable the D3D12 debug layer.
	{
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
		return true;
	}
#endif
	return false;
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

void AweD3D::InitFactory()
{
	UINT createFactoryFlags = 0;

	if (EnableDebugLayer())
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

	// Creating factory
	//ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&m_dxgiFactory)));
	ThrowIfFailed(CreateDXGIFactory( IID_PPV_ARGS(&m_dxgiFactory)));
}

int AweD3D::SelectAdapter() {
	// Listing the adapters
	UINT i = 0;
	IDXGIAdapter* pAdapter;
	while (m_dxgiFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		m_vcAdapters.push_back(pAdapter);
		++i;
	}

	// And Selecting one of them
	SIZE_T nMemSize = 0;
	int nSelectedAdapter = 0;
	for (int i = 0; i < m_vcAdapters.size(); i++) {
		DXGI_ADAPTER_DESC pDesc;
		HRESULT hr = m_vcAdapters.at(i)->GetDesc(&pDesc);

		if (pDesc.DedicatedVideoMemory > nMemSize) {
			nSelectedAdapter = i;
			nMemSize = pDesc.DedicatedVideoMemory;
		}	
	}
	return nSelectedAdapter;
}

void AweD3D::CreateDevice()
{
	// Selecting the adapter
	int nSelectedAdapter = SelectAdapter();

	// Try to create hardware device.
	HRESULT hardwareResult = D3D12CreateDevice(
		m_vcAdapters.at(nSelectedAdapter),
		D3D_FEATURE_LEVEL_12_0,
		IID_PPV_ARGS(&m_d3d12Device));

	// Fallback to WARP device.
	if (FAILED(hardwareResult))
	{
		Microsoft::WRL::ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_12_0,
			IID_PPV_ARGS(&m_d3d12Device)));
	}

	/*Microsoft::WRL::ComPtr<ID3D12Debug> spDebugController0;
	Microsoft::WRL::ComPtr<ID3D12Debug1> spDebugController1;
	D3D12GetDebugInterface(IID_PPV_ARGS(&spDebugController0));
	spDebugController0->QueryInterface(IID_PPV_ARGS(&spDebugController1));
	spDebugController1->SetEnableGPUBasedValidation(true);*/
	/*/ Enable debug messages in debug mode.
#if defined(_DEBUG)
	ComPtr<ID3D12InfoQueue> pInfoQueue;
	if (SUCCEEDED(m_d3d12Device.As(&pInfoQueue)))
	{
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
		// Suppress whole categories of messages
		//D3D12_MESSAGE_CATEGORY Categories[] = {};

		// Suppress messages based on their severity level
		D3D12_MESSAGE_SEVERITY Severities[] =
		{
			D3D12_MESSAGE_SEVERITY_INFO
		};

		// Suppress individual messages by their ID
		D3D12_MESSAGE_ID DenyIds[] = {
			D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
			D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
			D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
		};

		D3D12_INFO_QUEUE_FILTER NewFilter = {};
		//NewFilter.DenyList.NumCategories = _countof(Categories);
		//NewFilter.DenyList.pCategoryList = Categories;
		NewFilter.DenyList.NumSeverities = _countof(Severities);
		NewFilter.DenyList.pSeverityList = Severities;
		NewFilter.DenyList.NumIDs = _countof(DenyIds);
		NewFilter.DenyList.pIDList = DenyIds;

		ThrowIfFailed(pInfoQueue->PushStorageFilter(&NewFilter));
	}
#endif*/
}

void AweD3D::CreateCommandQueue()
{
	// Creating command queue 
	m_pCommandQueue = std::make_shared<AweD3DCommandQueue>(m_d3d12Device.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT);
}

void AweD3D::CheckTearingSupport()
{
	BOOL allowTearing = FALSE;

	// Rather than create the DXGI 1.5 factory interface directly, we create the
	// DXGI 1.4 interface and query for the 1.5 interface. This is to enable the 
	// graphics debugging tools which will not support the 1.5 factory interface 
	// until a future update.
	Microsoft::WRL::ComPtr<IDXGIFactory5> factory5;
	if (SUCCEEDED(m_dxgiFactory.As(&factory5)))
	{
		if (FAILED(factory5->CheckFeatureSupport(
			DXGI_FEATURE_PRESENT_ALLOW_TEARING,
			&allowTearing, sizeof(allowTearing))))
		{
			allowTearing = FALSE;
		}
	}
	

	m_bTearingSupported = (allowTearing == TRUE);
}

void AweD3D::InitDescriptorSizes()
{
	m_RtvDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_DsvDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_CbvSrvUavDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}
//-----------------------------------------------------------------------------
// Name: InitDirect3D
// Desc: Initialize the direct3D objects
//-----------------------------------------------------------------------------
bool AweD3D::InitDirect3D()
{

	InitFactory();
	CreateDevice();
	CreateCommandQueue();
	
	
	// Getting sizes of resource descriptors(views) as these are hardware dependant
	InitDescriptorSizes();

	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();

	InitUploadBuffers();

	// TODO: This will later will created dinamically, for now we use a default shader
	CreateDefaultRootSignature();
	CreateDefaultPipelineStateObject();

	InitSkinManager();
	InitVertexManager();

	return true;
}

void AweD3D::InitSkinManager() {
	m_SkinManager.Init(m_d3d12Device, m_pCommandQueue);
}

void AweD3D::InitVertexManager() {
	m_VertexManager.Init(m_d3d12Device, m_pCommandQueue, &m_SkinManager, this);
}

void AweD3D::CreateDefaultRootSignature() {

	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[2];

	// Create root CBV.
	slotRootParameter[0].InitAsConstantBufferView(0);
	slotRootParameter[1].InitAsConstantBufferView(1);
	//slotRootParameter[2].InitAsConstantBufferView(2);


	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(2, slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(m_d3d12Device->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(m_rootSignature.GetAddressOf())));


	// Create a root signature.
	/*D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	if (FAILED(m_d3d12Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
	{
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	// Allow input layout and deny unnecessary access to certain pipeline stages.
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS ;

	// A single 32-bit constant root parameter that is used by the vertex shader.
	CD3DX12_ROOT_PARAMETER1 rootParameters[2];

	CD3DX12_DESCRIPTOR_RANGE1 cbvTable0;
	cbvTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	CD3DX12_DESCRIPTOR_RANGE1 cbvTable1;
	cbvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

	rootParameters[0].InitAsDescriptorTable(1, &cbvTable0);
	rootParameters[1].InitAsDescriptorTable(1, &cbvTable1);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
	rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

	// Serialize the root signature.
	Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
		featureData.HighestVersion, &rootSignatureBlob, &errorBlob));
	// Create the root signature.
	ThrowIfFailed(m_d3d12Device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
		rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));*/
}

void AweD3D::CreateDefaultPipelineStateObject()
{
/*#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	std::wstring str = GetAssetFullPath(L"DefaultVertexShader.cso");
	// Load the vertex shader.
	ThrowIfFailed(D3DReadFileToBlob(GetAssetFullPath(L"DefaultVertexShader.cso").c_str(), &m_vertexShader));

	// Load the pixel shader.
	ThrowIfFailed(D3DReadFileToBlob(GetAssetFullPath(L"DefaultPixelShader.cso").c_str(), &m_pixelShader));*/

	CreatePipelineStateObject(L"DefaultVertexShader.cso", L"DefaultPixelShader.cso", false, m_nPipelineStateIndex);
}


void AweD3D::CreateSwapChain()
{
	CheckTearingSupport();

	// Release the previous swapchain we will be recreating.
	m_SwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC1 sd;
	sd.Width = m_nClientWidth;
	sd.Height = m_nClientHeight;
	sd.Format = m_BackBufferFormat;
	sd.Stereo = FALSE;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = sm_nSwapChainBufferCount;
	sd.Scaling = DXGI_SCALING_STRETCH;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	sd.Flags = m_bTearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

	// Note: Swap chain uses queue to perform flush.
	ThrowIfFailed(m_dxgiFactory->CreateSwapChainForHwnd(
		m_pCommandQueue->GetD3D12CommandQueue().Get(),
		m_hWnd,
		&sd,
		nullptr,
		nullptr,
		m_SwapChain.GetAddressOf()));

	// Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
	// will be handled manually.
	ThrowIfFailed(m_dxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER));
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> AweD3D::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, 
	D3D12_DESCRIPTOR_HEAP_FLAGS flags, uint32_t numDescriptors)
{
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = numDescriptors;
	desc.Type = type;
	desc.Flags = flags;
	desc.NodeMask = 0;

	ThrowIfFailed(m_d3d12Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)));

	return descriptorHeap;
}

void AweD3D::CreateRtvAndDsvDescriptorHeaps()
{
	m_RtvHeap = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE,sm_nSwapChainBufferCount);
	m_DsvHeap = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);

	//m_CbvHeapPassVariables = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 1);
	//m_CbvHeapPerObjectVariables = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 1);
}

void AweD3D::InitUploadBuffers()
{
	// Per pass
	m_CbUploadPassVariables = std::make_unique<AweD3DUploadBuffer<AWEPASSVARIABLES>>(m_d3d12Device.Get(), 1, true);

	UINT objCBByteSize = CalcConstantBufferByteSize(sizeof(AWEPASSVARIABLES));

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = m_CbUploadPassVariables->Resource()->GetGPUVirtualAddress();
	
	// Offset to the ith object constant buffer in the buffer.
	// int boxCBufIndex = 0;
	// cbAddress += boxCBufIndex * objCBByteSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = objCBByteSize;

	////m_d3d12Device->CreateConstantBufferView(&cbvDesc,
		//m_CbvHeapPassVariables->GetCPUDescriptorHandleForHeapStart());

	// Per Object
	m_CbUploadPerObjectVariables = std::make_unique<AweD3DUploadBuffer<AWEPEROBJECTVARIABLES>>(m_d3d12Device.Get(), 1, true);

	objCBByteSize = CalcConstantBufferByteSize(sizeof(AWEPEROBJECTVARIABLES));

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress2 = m_CbUploadPerObjectVariables->Resource()->GetGPUVirtualAddress();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc2;
	cbvDesc2.BufferLocation = cbAddress2;
	cbvDesc2.SizeInBytes = objCBByteSize;

	//m_d3d12Device->CreateConstantBufferView(&cbvDesc2,
		//m_CbvHeapPerObjectVariables->GetCPUDescriptorHandleForHeapStart());
}