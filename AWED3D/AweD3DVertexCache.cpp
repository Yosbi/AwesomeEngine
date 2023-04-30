/*/-----------------------------------------------------------------------
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
	std::shared_ptr<AweD3DSkinManager> pSkinMan, std::shared_ptr<AweD3D> pAweD3D) :
	m_pDevice(pDevice),
	m_CommandQueue(pCommandQueue),
	m_pSkinMan(pSkinMan),
	m_pAweD3D(pAweD3D)
{
}
//-----------------------------------------------------------------------------
// Name: CreateStaticBuffer(Manager)
// Desc: Create a static vertex/index buffer for the given data and returns
//		 a handle to that buffer for later rendering processes
//-----------------------------------------------------------------------------
HRESULT AweD3DVertexCacheManager::CreateStaticBuffer(UINT nSkinID, UINT nVerts,
	UINT nIndis, const void* pVerts, const WORD* pIndis, UINT* pnID)
{
	HRESULT  hr;
	void* pData;
	//Log("usa shader %i", m_pYD3D->UsesShaders());
	if (m_nNumSB >= (MAX_ID - 1)) return Y_OUTOFMEMORY;

	// Allocate memory for static buffers if needed
	if ((m_nNumSB % 50) == 0)
	{
		int n = (m_nNumSB + 50) * sizeof(YSTATICBUFFER);
		m_pSB = (YSTATICBUFFER*)realloc(m_pSB, n);
		if (!m_pSB) return Y_OUTOFMEMORY;
	}

	m_pSB[m_nNumSB].nNumVerts = nVerts;
	m_pSB[m_nNumSB].nNumIndis = nIndis;
	m_pSB[m_nNumSB].nSkinID = nSkinID;
	m_pSB[m_nNumSB].pIB = NULL;
	m_pSB[m_nNumSB].pVB = NULL;
	m_pSB[m_nNumSB].VID = VertexID;

	// Get size and format of vertex
	switch (VertexID) {
	case VID_PS: {
		m_pSB[m_nNumSB].nStride = sizeof(PVERTEX);
	} break;
	case VID_UU: {
		m_pSB[m_nNumSB].nStride = sizeof(VERTEX);
	} break;
	case VID_UL: {
		m_pSB[m_nNumSB].nStride = sizeof(LVERTEX);
	} break;
	case VID_CA: {
		m_pSB[m_nNumSB].nStride = sizeof(CVERTEX);
	} break;
	case VID_3T: {
		m_pSB[m_nNumSB].nStride = sizeof(VERTEX3T);
	} break;
	case VID_TV: {
		m_pSB[m_nNumSB].nStride = sizeof(TVERTEX);
	} break;

	default: return Y_INVALIDID;
	}

	// Create indexbuffer if needed
	if (nIndis > 0)
	{
		m_pSB[m_nNumSB].bIndis = true;
		m_pSB[m_nNumSB].nNumTris = int(nIndis / 3.0f);
		hr = m_pDevice->CreateIndexBuffer(nIndis * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
			D3DPOOL_DEFAULT, &m_pSB[m_nNumSB].pIB, NULL);
		if (FAILED(hr))
		{
			if (g_bLF)Log("Error creating static index buffer");
			return Y_CREATEBUFFER;
		}

		// Fill the index buffer
		if (SUCCEEDED(m_pSB[m_nNumSB].pIB->Lock(0, 0, (void**)(&pData), 0)))
		{
			memcpy(pData, pIndis, nIndis * sizeof(WORD));
			m_pSB[m_nNumSB].pIB->Unlock();
		}
		else
		{
			if (g_bLF)Log("Error in lock call creating static index buffer");
			return Y_BUFFERLOCK;
		}
	}
	else
	{
		m_pSB[m_nNumSB].bIndis = false;
		m_pSB[m_nNumSB].nNumTris = int(nVerts / 3.0f);
		m_pSB[m_nNumSB].pIB = NULL;
	}

	// Create vertex buffer	
	hr = m_pDevice->CreateVertexBuffer(nVerts * m_pSB[m_nNumSB].nStride, D3DUSAGE_WRITEONLY, 0,
		D3DPOOL_DEFAULT, &m_pSB[m_nNumSB].pVB, NULL);
	if (FAILED(hr))
	{
		if (g_bLF)Log("Error creating static vertex buffer");
		return Y_CREATEBUFFER;
	}

	// Fill the vertex buffer
	if (SUCCEEDED(m_pSB[m_nNumSB].pVB->Lock(0, 0, (void**)(&pData), 0)))
	{
		memcpy(pData, pVerts, nVerts * m_pSB[m_nNumSB].nStride);
		m_pSB[m_nNumSB].pVB->Unlock();
	}
	else
	{
		if (g_bLF)Log("Error in lock call creating static vertex buffer");
		return Y_BUFFERLOCK;
	}

	if (pnID)(*pnID) = m_nNumSB;
	m_nNumSB++;
	return Y_OK;
}

//-----------------------------------------------------------------------------
// Name: SetVertexDeclaration
// Desc: Sets the vertex declaration
//-----------------------------------------------------------------------------
HRESULT AweD3DVertexCacheManager::SetVertexDeclaration(DWORD VertexID)
{
	// check
	if (m_dwActiveVertexDcl == VertexID)
		return Y_OK;
	else
	{
		// clear all vertex caches
//	m_pVertexManager->ForcedFlushAll();
////   m_pVertexMan->InvalidateStates();	
	}

	switch (VertexID)
	{
	case VID_PS: {
		if (FAILED(m_pDevice->SetVertexDeclaration(m_pYD3D->m_pDeclPVertex)))
			return Y_FAIL;
	} break;
	case VID_UU: {
		if (FAILED(m_pDevice->SetVertexDeclaration(m_pYD3D->m_pDeclVertex)))
			return Y_FAIL;
	} break;
	case VID_UL: {
		if (FAILED(m_pDevice->SetVertexDeclaration(m_pYD3D->m_pDeclLVertex)))
			return Y_FAIL;
	} break;
	case VID_CA: {
		if (FAILED(m_pDevice->SetVertexDeclaration(m_pYD3D->m_pDeclCVertex)))
			return Y_FAIL;
	} break;
	case VID_3T: {
		if (FAILED(m_pDevice->SetVertexDeclaration(m_pYD3D->m_pDecl3TVertex)))
			return Y_FAIL;
	} break;
	case VID_TV: {
		if (FAILED(m_pDevice->SetVertexDeclaration(m_pYD3D->m_pDeclTVertex)))
			return Y_FAIL;
	} break;
	default: return Y_INVALIDID;
	}

	// Set the currently used vertex declaration
	m_dwActiveVertexDcl = VertexID;
	return Y_OK;
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

}*/