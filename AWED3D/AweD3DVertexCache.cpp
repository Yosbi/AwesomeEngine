//-----------------------------------------------------------------------
// AweD3DException.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 30/04/2023
//-----------------------------------------------------------------------
//Desc: Implementation of the AweD3DVertexCacheManager and AweD3DVertexCache

//----------------------------------------------------------------------
// includes
//----------------------------------------------------------------------
#include "AweD3DVertexCache.h" // Class definition

//-----------------------------------------------------------------------------
// Name: Constructor(Manager)
// Desc: Initializes all the variables
//-----------------------------------------------------------------------------
AweD3DVertexCacheManager::AweD3DVertexCacheManager()
{
	m_dwActiveSB = MAX_ID;
}

//-----------------------------------------------------------------------------
// Name: Destructor(Manager)
// Desc: Releases vertex cache arrays
//-----------------------------------------------------------------------------
AweD3DVertexCacheManager::~AweD3DVertexCacheManager()
{
}

//-----------------------------------------------------------------------------
// Name: Init(Manager)
// Desc: Crank up the Vertex Cache manager
//-----------------------------------------------------------------------------
void AweD3DVertexCacheManager::Init(Microsoft::WRL::ComPtr<ID3D12Device2> pDevice, std::shared_ptr<AweD3DCommandQueue> pCommandQueue,
	std::shared_ptr<AweD3DSkinManager> pSkinMan, std::shared_ptr<AweD3D> pAweD3D) 
{
	m_pDevice = pDevice;
	m_pCommandQueue = pCommandQueue;
	m_pSkinMan = pSkinMan;
	m_pAweD3D = pAweD3D;
}
//-----------------------------------------------------------------------------
// Name: CreateStaticBuffer(Manager)
// Desc: Create a static vertex/index buffer for the given data and returns
//		 a handle to that buffer for later rendering processes
//-----------------------------------------------------------------------------
HRESULT AweD3DVertexCacheManager::CreateStaticBuffer(AWESOMEVERTEXID VertexID, UINT nSkinID, UINT nVerts,
	UINT nIndis, const void* pVerts, const WORD* pIndis, UINT* pnID)
{
	HRESULT  hr;
	AWESOMESTATICBUFFER awesomeStaticBuffer;

	awesomeStaticBuffer.nNumVerts = nVerts;
	awesomeStaticBuffer.nNumIndis = nIndis;
	awesomeStaticBuffer.nSkinID = nSkinID;
	awesomeStaticBuffer.VID = VertexID;

	// Get size and format of vertex
	switch (VertexID) {
		case VID_PS: {
			awesomeStaticBuffer.nStride = sizeof(PVERTEX);
		} break;
		case VID_UU: {
			awesomeStaticBuffer.nStride = sizeof(VERTEX);
		} break;
		case VID_UL: {
			awesomeStaticBuffer.nStride = sizeof(LVERTEX);
		} break;
		default: return E_INVALIDARG;
	}

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList = m_pCommandQueue->GetCommandList();

	// Create indexbuffer if needed
	if (nIndis > 0)
	{
		awesomeStaticBuffer.bIndis = true;
		awesomeStaticBuffer.nNumTris = int(nIndis / 3.0f);
	
		// Upload index buffer data to GPU.
		Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> intermediateIndexBuffer;
		UpdateBufferResource(commandList,
			&indexBuffer, &intermediateIndexBuffer,
			nIndis, sizeof(WORD), pIndis);

		// Create index buffer view.
		D3D12_INDEX_BUFFER_VIEW* indexBufferView = &awesomeStaticBuffer.indexBufferView;
		indexBufferView->BufferLocation = indexBuffer->GetGPUVirtualAddress();
		indexBufferView->Format = DXGI_FORMAT_R16_UINT;
		indexBufferView->SizeInBytes = (unsigned int)(nIndis * sizeof(WORD));

		awesomeStaticBuffer.indexBuffer = indexBuffer;
	}
	else
	{
		awesomeStaticBuffer.bIndis = false;
		awesomeStaticBuffer.nNumTris = int(nVerts / 3.0f);
	}

	// Upload vertex buffer data to GPU.
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateVertexBuffer;
	UpdateBufferResource(commandList,
		&vertexBuffer, &intermediateVertexBuffer,
		nVerts, awesomeStaticBuffer.nStride, pVerts);

	// Create the vertex buffer view.
	D3D12_VERTEX_BUFFER_VIEW* vertexBufferView = &awesomeStaticBuffer.vertexBufferView;
	vertexBufferView->BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView->SizeInBytes = (unsigned int)(nVerts * awesomeStaticBuffer.nStride);
	vertexBufferView->StrideInBytes = awesomeStaticBuffer.nStride;

	awesomeStaticBuffer.vertexBuffer = vertexBuffer;

	m_pSB.push_back(awesomeStaticBuffer);

	m_pCommandQueue->ExecuteCommandList(commandList);
	m_pCommandQueue->Flush();
	return S_OK;
}

void AweD3DVertexCacheManager::UpdateBufferResource(
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
	ID3D12Resource** pDestinationResource,
	ID3D12Resource** pIntermediateResource,
	size_t numElements, size_t elementSize, const void* bufferData)
{
	size_t bufferSize = numElements * elementSize;

	// Create a committed resource for the GPU resource in a default heap.
	ThrowIfFailed(m_pDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_NONE),
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(pDestinationResource)));

	// Create an committed resource for the upload.
	if (bufferData)
	{
		ThrowIfFailed(m_pDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(pIntermediateResource)));

		D3D12_SUBRESOURCE_DATA subresourceData = {};
		subresourceData.pData = bufferData;
		subresourceData.RowPitch = bufferSize;
		subresourceData.SlicePitch = subresourceData.RowPitch;

		UpdateSubresources(commandList.Get(),
			*pDestinationResource, *pIntermediateResource,
			0, 0, 1, &subresourceData);
	}
}


//-----------------------------------------------------------------------------
// Name: Render(Manager)
// Desc: Render buffers with the given skin. Index buffers is optional.
//-----------------------------------------------------------------------------
HRESULT AweD3DVertexCacheManager::Render(UINT nID)
{
	HRESULT hr = S_OK;
	int		iT = 0;

	if (!m_pAweD3D->IsSceneRunning())
	{
		return E_FAIL;
	}

	//YRENDERSTATE sm = m_pYD3D->GetShadeMode();


	if (nID >= m_pSB.size())
	{
		return E_INVALIDARG;
	}

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList = m_pCommandQueue->GetCommandList();
	
	commandList->RSSetViewports(1, m_pAweD3D->GetScreenViewPort());
	commandList->RSSetScissorRects(1, m_pAweD3D->GetSissorRect());

	// Specify the buffers we are going to render to.
	commandList->OMSetRenderTargets(1, &m_pAweD3D->getCurrentBackBufferView(), true, &m_pAweD3D->getDepthStencilView());

	commandList->SetPipelineState(m_pAweD3D->m_pipelineState.Get());
	commandList->SetGraphicsRootSignature(m_pAweD3D->m_rootSignature.Get());

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Setting the matrixes in the shader

	DirectX::XMMATRIX mvpMatrix = m_pAweD3D->GetMVPMatrix();
	commandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &mvpMatrix, 0);

	// Activate buffers if not already active
	if (m_dwActiveSB != nID)
	{
		// Index buffer used?
		if (m_pSB[nID].bIndis) 
			commandList->IASetIndexBuffer(&m_pSB[nID].indexBufferView);

		// Set the vertex buffer
		commandList->IASetVertexBuffers(0, 1, &m_pSB[nID].vertexBufferView);
		m_dwActiveSB = nID;
	}
	
	// Is the device already using this skin?   
	if (m_pAweD3D->GetActiveSkinID() != m_pSB[nID].nSkinID)
	{
		// Set material for device
		AWESOMESKIN* pSkin = &m_pSkinMan->m_Skins[m_pSB[nID].nSkinID];

		// TODO: Set skin in shader
		
		// Skin will change now
		m_pAweD3D->SetActiveSkinID(m_pSB[nID].nSkinID);
	}

	
	// Finally draw
	// Indexed primitive
	if (m_pSB[nID].bIndis)
	{
		commandList->DrawIndexedInstanced(m_pSB[nID].nNumIndis, 1, 0, 0, 0);
	}
	// Non-indexed primitive
	else
	{
		commandList->DrawInstanced(m_pSB[nID].nNumVerts, 1, 0, 0);
	}

	return hr;

}