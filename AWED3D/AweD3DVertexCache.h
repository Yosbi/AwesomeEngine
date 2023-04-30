/*/-----------------------------------------------------------------------
// AweD3DException.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 30/04/2023
//-----------------------------------------------------------------------
// Desc: This file contains datatypes and clases to implement a the 
// rendering part of the AwesomeEngine, it contains a vertex cache class and
// a vertex cache manager.

#ifndef AWED3DVERTEXCACHE_H
#define AWED3DVERTEXCACHE_H
#pragma once
//----------------------------------------------------------------------
// includes
//----------------------------------------------------------------------
#include "AweD3D.h"
#include "AweD3DSkinManager.h"

//----------------------------------------------------------------------
// Name: YSTATICBUFFER
// Desc: Struct for static vertex/index data
//----------------------------------------------------------------------
typedef struct AWESTATICBUFFER_TYPE
{
	int			nStride;
	UINT		nSkinID;
	bool		bIndis;
	int			nNumVerts;
	int			nNumIndis;
	int			nNumTris;
	LPDIRECT3DVERTEXBUFFER9 pVB;
	LPDIRECT3DINDEXBUFFER9	pIB;
} AWESOMESTATICBUFFER;

//----------------------------------------------------------------------
// Name: AweD3DVertexCacheManager
// Desc: Class to manage static and dynamic vertex bunches, optionally
//		 using indices during rendering process.
//----------------------------------------------------------------------
class AweD3DVertexCacheManager : public AwesomeVertexCacheManager
{
public:
	// Cons/Dest
	AweD3DVertexCacheManager();
	~AweD3DVertexCacheManager();

	void Init(Microsoft::WRL::ComPtr<ID3D12Device2> pDevice, std::shared_ptr<AweD3DCommandQueue> pCommandQueue, 
		std::shared_ptr<AweD3DSkinManager> pSkinMan, std::shared_ptr<AweD3D> pAweD3D);

	// Public functions
	HRESULT		 CreateStaticBuffer(UINT nSkinID, UINT nVerts, UINT nIdis, const void* pVerts,
		const WORD* pIndis, UINT* pnID);
	HRESULT		 Render(UINT nSBufferID);

private:
	// Private functions	
	HRESULT		 SetVertexDeclaration(DWORD VertexID);

	// Private member vars
	Microsoft::WRL::ComPtr<ID3D12Device2> m_pDevice;	// Direct3d device
	std::shared_ptr<AweD3DCommandQueue> m_CommandQueue; // Command queue
	std::shared_ptr<AweD3DSkinManager> m_pSkinMan;		// The skin manager
	std::shared_ptr<AweD3D> m_pAweD3D;					// 

	std::vector<AWESOMESTATICBUFFER> m_pSB;			// Array of Static vertex buffer
	DWORD				 m_dwActiveSB;				// The currently active static buffer
};

#endif*/