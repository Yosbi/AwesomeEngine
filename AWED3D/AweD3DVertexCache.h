//-----------------------------------------------------------------------
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
#include "Awe.h"
#include "AweD3DSkinManager.h"
#include <memory>


class AweD3D; // Forward declaration


enum AWEBUFFERTYPE
{
	DYNAMIC = 0,
	STATIC = 1

};

//----------------------------------------------------------------------
// Name: AWESTATICBUFFER
// Desc: Struct for static vertex/index data
// TODO: Encapsulate better this in class with its own methods for the
// static and dynamic buffers
//----------------------------------------------------------------------
class AWESOMESTATICBUFFER
{
public:
	int			nStride;
	UINT		nSkinID;
	bool		bIndis;
	int			nNumVerts;
	int			nNumIndis;
	int			nNumTris;
	AWEBUFFERTYPE BufferType;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBufferGPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBufferUploader = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBufferUploader = nullptr;

	// System memory copies.  Use Blobs because the vertex/index format can be generic.
	// It is up to the client to cast appropriately.  
	Microsoft::WRL::ComPtr<ID3DBlob> vertexBufferCPU = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> indexBufferCPU = nullptr;

	// If we have a dynamic buffer we upload it here
	// TODO: Allow flexibility on the vertex type here
	std::shared_ptr<AweD3DUploadBuffer<VERTEX>> m_DynamicVertexUploadBuffer = nullptr;	// Upload buffer

} ;

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
		AweD3DSkinManager* pSkinMan, AweD3D* pAweD3D);

	// Public functions
	HRESULT		 CreateStaticBuffer(UINT nSkinID, UINT nVerts, int nVertsStride, const void* pVerts,
		UINT nIndis, const WORD* pIndis, UINT* pnID);
	HRESULT		 CreateDynamicBuffer(UINT nSkinID, UINT nVerts, VERTEX* pVerts,
		UINT nIndis, const WORD* pIndis, UINT* pnID);
	HRESULT		UpdateDynamicBuffer(UINT nSBufferID, VERTEX* pVerts);
	HRESULT		Render(UINT nSBufferID);

private:
	// Private functions	
	void UpdateBufferResource(
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
		ID3D12Resource** pDestinationResource,
		ID3D12Resource** pIntermediateResource,
		size_t numElements, size_t elementSize, const void* bufferData);

	// Private member vars
	Microsoft::WRL::ComPtr<ID3D12Device2> m_pDevice;		// Direct3d device
	std::shared_ptr<AweD3DCommandQueue> m_pCommandQueue;	// Command queue
	AweD3DSkinManager* m_pSkinMan;							// The skin manager
	AweD3D* m_pAweD3D;										// The AweD3D

	std::vector<AWESOMESTATICBUFFER> m_pSB;					// Array of Static vertex buffer
};

#endif