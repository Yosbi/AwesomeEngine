//-----------------------------------------------------------------------
// AweD3D.cpp
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 11/03/2023
//-----------------------------------------------------------------------
#include "AweD3D.h"


bool AweD3D::IsRunning()
{
	return false;
}

AwesomeSkinManager* AweD3D::GetSkinManager()
{
	return &m_SkinManager;
}

AwesomeVertexCacheManager* AweD3D::GetVertexManager()
{
	return &m_VertexManager;
}

HRESULT AweD3D::BeginRendering(bool bClearPixel, bool bClearDepth, bool bClearStencil)
{
	m_bIsSceneRunning = true;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList = m_pCommandQueue->GetCommandList();

	// Indicate a state transition on the resource usage.
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(getCurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &barrier);

	// Clear the back buffer and depth buffer.
	commandList->ClearRenderTargetView(getCurrentBackBufferView(), m_ClearColor, 0, nullptr);
	commandList->ClearDepthStencilView(getDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	m_pCommandQueue->ExecuteCommandList(commandList);

	// As we are starting the pass, we still havent setted the pass variables
	m_bSettedPassVariablesCB = false;

	m_pCommandQueue->Flush();

	return S_OK;
}

void AweD3D::EndRendering()
{
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList = m_pCommandQueue->GetCommandList();

	// Draw the triangles
	//DrawTriangles();

	// Indicate a state transition on the resource usage.
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(getCurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	commandList->ResourceBarrier(1, &barrier);

	m_pCommandQueue->ExecuteCommandList(commandList);
	
	// swap the back and front buffers
	UINT syncInterval = m_bVSync ? 1 : 0;
	UINT presentFlags = m_bTearingSupported && !m_bVSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
	ThrowIfFailed(m_SwapChain->Present(syncInterval, presentFlags)); // (1, 0)
	m_nCurrBackBuffer = (m_nCurrBackBuffer + 1) % sm_nSwapChainBufferCount;

	m_pCommandQueue->Flush();
	
	m_bIsSceneRunning = false;
}

HRESULT AweD3D::Clear(bool bClearPixel, bool bClearDepth, bool bClearStencil)
{
	return E_NOTIMPL;
}

void AweD3D::SetClearColor(float fRed, float fGreen, float fBlue)
{
	m_ClearColor[0] = fRed;
	m_ClearColor[1] = fGreen;
	m_ClearColor[2] = fBlue;
	m_ClearColor[3] = 1.0f;
}

bool AweD3D::IsSceneRunning()
{
	return m_bIsSceneRunning;
}

D3D12_VIEWPORT* AweD3D::GetScreenViewPort()
{
	return &m_ScreenViewport;
}

D3D12_RECT* AweD3D::GetSissorRect()
{
	return &m_ScissorRect;
}

HRESULT AweD3D::Go()
{
	// TODO: do a dialog box to select settings (screen resolution, gpu settings, etc)
	return E_NOTIMPL;
}

void AweD3D::Log(std::wstring, ...)
{
	// TODO: do a log
}

void AweD3D::ResizeSwapChain()
{
	// Resize the swap chain.
	ThrowIfFailed(m_SwapChain->ResizeBuffers(
		sm_nSwapChainBufferCount,
		m_nClientWidth, m_nClientHeight,
		m_BackBufferFormat,
		m_bTearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0));

	m_nCurrBackBuffer = 0;
}

void AweD3D::UpdateRenderTargetView()
{
	// Release the previous resources we will be recreating.
	for (int i = 0; i < sm_nSwapChainBufferCount; ++i)
		m_SwapChainBuffer[i].Reset();

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < sm_nSwapChainBufferCount; i++)
	{
		ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_SwapChainBuffer[i])));
		m_d3d12Device->CreateRenderTargetView(m_SwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, m_RtvDescriptorSize);
	}
}

void AweD3D::UpdateDepthStencilView()
{
	m_DepthStencilBuffer.Reset();

	// Create the depth/stencil buffer and view.
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = m_nClientWidth;
	depthStencilDesc.Height = m_nClientHeight;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = m_DepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	ThrowIfFailed(m_d3d12Device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(m_DepthStencilBuffer.GetAddressOf())));

	// Create descriptor to mip level 0 of entire resource using the format of the resource.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = m_DepthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	m_d3d12Device->CreateDepthStencilView(m_DepthStencilBuffer.Get(), &dsvDesc, getDepthStencilView());

	// Transition the resource from its initial state to be used as a depth buffer.
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList = m_pCommandQueue->GetCommandList();

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	commandList->ResourceBarrier(1, &barrier);

	// Execute the resize commands.
	m_pCommandQueue->ExecuteCommandList(commandList);

	// Wait until resize is complete.
	m_pCommandQueue->Flush();
}

void AweD3D::OnResize()
{
	assert(m_d3d12Device);
	assert(m_SwapChain);

	// Flush before changing any resources.
	m_pCommandQueue->Flush();

	ResizeSwapChain();
	UpdateRenderTargetView();
	UpdateDepthStencilView();

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

D3D12_CPU_DESCRIPTOR_HANDLE AweD3D::getDepthStencilView() const
{
	return m_DsvHeap->GetCPUDescriptorHandleForHeapStart();
}

D3D12_CPU_DESCRIPTOR_HANDLE AweD3D::getCurrentBackBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		m_RtvHeap->GetCPUDescriptorHandleForHeapStart(),
		m_nCurrBackBuffer,
		m_RtvDescriptorSize);
}

ID3D12Resource* AweD3D::getCurrentBackBuffer() const
{
	return m_SwapChainBuffer[m_nCurrBackBuffer].Get();
}

void AweD3D::SetAmbientLight(AWESOMECOLOR color)
{
	m_PassVariables.cAmbientLight = color;
}
void AweD3D::SetDiffuseLight(AWESOMECOLOR color)
{
	m_PassVariables.cDiffuseLight = color;
}
void AweD3D::SetSpecularLight(AWESOMECOLOR color)
{
	m_PassVariables.cSpecularLight = color;
}
void AweD3D::SetLightDirection(AWEVector vcDir)
{
	m_PassVariables.vcLightVecW = vcDir;
}