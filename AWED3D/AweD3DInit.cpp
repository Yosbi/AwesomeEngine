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
	m_pCommandQueue(nullptr),
	m_bVSync(true),
	m_bTearingSupported(false),
	m_fFoV(45.0f),
	m_fNear(0.1f),
	m_fFar(100.0f),
	m_ProjectionMatrix(DirectX::XMMatrixIdentity()),
	m_ViewMatrix(DirectX::XMMatrixIdentity()),
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

void AweD3D::CreateDevice()
{
	// Try to create hardware device.
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,             // default adapter
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

	// TODO: This will later will created dinamically, for now we use a default shader
	CreateDefaultRootSignature();
	LoadDefaultShaders();
	CreateDefaultPipelineStateObject();

	InitSkinManager();

	return true;
}

void AweD3D::InitSkinManager() {
	m_SkinManager.Init(m_d3d12Device, m_pCommandQueue);
}
void AweD3D::CreateDefaultRootSignature() {
	// Create a root signature.
	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
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
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	// A single 32-bit constant root parameter that is used by the vertex shader.
	CD3DX12_ROOT_PARAMETER1 rootParameters[1];
	rootParameters[0].InitAsConstants(sizeof(DirectX::XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
	rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

	// Serialize the root signature.
	Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
		featureData.HighestVersion, &rootSignatureBlob, &errorBlob));
	// Create the root signature.
	ThrowIfFailed(m_d3d12Device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
		rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
}

void AweD3D::LoadDefaultShaders() 
{
#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	std::wstring str = GetAssetFullPath(L"DefaultVertexShader.cso");
	// Load the vertex shader.
	ThrowIfFailed(D3DReadFileToBlob(GetAssetFullPath(L"DefaultVertexShader.cso").c_str(), &m_vertexShader));

	// Load the pixel shader.
	ThrowIfFailed(D3DReadFileToBlob(GetAssetFullPath(L"DefaultPixelShader.cso").c_str(), &m_pixelShader));
}

void AweD3D::CreateDefaultPipelineStateObject()
{
	// Define the vertex input layout.
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		//{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	// Describe and create the graphics pipeline state object (PSO).
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = m_rootSignature.Get();
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vertexShader.Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_pixelShader.Get());
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.DSVFormat = m_DepthStencilFormat;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	ThrowIfFailed(m_d3d12Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
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

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> AweD3D::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors)
{
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = numDescriptors;
	desc.Type = type;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	desc.NodeMask = 0;

	ThrowIfFailed(m_d3d12Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)));

	return descriptorHeap;
}

void AweD3D::CreateRtvAndDsvDescriptorHeaps()
{
	m_RtvHeap = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, sm_nSwapChainBufferCount);
	m_DsvHeap = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);
}