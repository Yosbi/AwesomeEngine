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
	awesomeStaticBuffer.vertexIdType = VertexID;

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
	HRESULT hr = Y_OK;
	int		iT = 0;

	if (!m_pYD3D->IsSceneRunning())
	{
		if (g_bLF) Log("Error: can not render because the scene is not running call begin()");
		return Y_FAIL;
	}

	//YRENDERSTATE sm = m_pYD3D->GetShadeMode();

	// Active cache gets invalid
	m_dwActiveCache = MAX_ID;

	if (nID >= m_nNumSB)
	{
		if (g_bLF)Log("error: invalid static buffer ID");
		return Y_INVALIDPARAM;
	}


	// Activate buffers if not already active
	if (m_dwActiveSB != nID)
	{
		// Index buffer used?
		if (m_pSB[nID].bIndis) m_pDevice->SetIndices(m_pSB[nID].pIB);
		// Set the vertex buffer
		m_pDevice->SetStreamSource(0, m_pSB[nID].pVB, 0, m_pSB[nID].nStride);
		// Set the vertex declaration
		if (FAILED(SetVertexDeclaration(m_pSB[nID].VID)))
		{
			if (g_bLF)Log("Error: SetVertexDeclaration");
			return Y_FAIL;
		}
		m_dwActiveSB = nID;
	}
	// There is maybe just another indexbuffer active
	else if (m_dwActiveIB != MAX_ID)
	{
		if (m_pSB[nID].bIndis) m_pDevice->SetIndices(m_pSB[nID].pIB);
		m_dwActiveIB = MAX_ID;
	}


	// Is the device already using this skin?   
	if (m_pYD3D->GetActiveSkinID() != m_pSB[nID].nSkinID)
	{
		// Set material for device
		YSKIN* pSkin = &m_pSkinMan->m_pSkins[m_pSB[nID].nSkinID];

		// Set material for the FX
		YMATERIAL* pMat = &m_pSkinMan->m_pMaterials[pSkin->nMaterial];
		m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].FX->SetValue(m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].hDiffuseMat, &pMat->cDiffuse, sizeof(YCOLOR));
		m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].FX->SetValue(m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].hAmbientMat, &pMat->cAmbient, sizeof(YCOLOR));
		m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].FX->SetValue(m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].hEmissiveMat, &pMat->cEmissive, sizeof(YCOLOR));
		m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].FX->SetValue(m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].hSpecMat, &pMat->cSpecular, sizeof(YCOLOR));
		m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].FX->SetFloat(m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].hSpecPowMat, pMat->fPower);

		// Set texture for device
		for (iT = 0; iT < 8; iT++)
		{
			if (pSkin->nTexture[iT] != MAX_ID)
				m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].FX->SetTexture(m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].hTex[iT], (LPDIRECT3DTEXTURE9)m_pSkinMan->m_pTextures[pSkin->nTexture[iT]].pData);
			else
				break;
		}
		m_pDevice->SetTextureStageState(iT, D3DTSS_COLOROP, D3DTOP_DISABLE);


		// Set alpha states for device
		if (pSkin->bAlpha)
		{
			m_pDevice->SetRenderState(D3DRS_ALPHAREF, 15);
			m_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
			m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		}
		else
		{
			m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}
		// Skin will change now
		m_pYD3D->SetActiveSkinID(m_pSB[nID].nSkinID);
	}


	//  Should I use additive rendering?
	if (m_pYD3D->UsesAdditiveBlending())
	{
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}
	//  Should I use rendering to color buffer at all?
	else if (!m_pYD3D->UsesColorBuffer())
	{
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}
	//// Should I force alpha blending?
	//else if(m_pYD3D->UsesAlphaBlending())
	//{
	//	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	//	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	//}


	// Indexed primitive
	if (m_pSB[nID].bIndis)
	{

		for (int i = 0; i < m_pYD3D->m_nPasses; i++)
		{
			//Begin the pass
			m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].FX->BeginPass(i);

			// Render
			hr = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_pSB[nID].nNumVerts, 0, m_pSB[nID].nNumTris);

			// End the pass
			m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].FX->EndPass();
		}

	}
	// Non-indexed primitive
	else
	{

		for (int i = 0; i < m_pYD3D->m_nPasses; i++)
		{
			m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].FX->BeginPass(i);

			// Render content			
			hr = m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_pSB[nID].nNumTris);

			// End the pass
			m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].FX->EndPass();
		}

	}

	return hr;

}