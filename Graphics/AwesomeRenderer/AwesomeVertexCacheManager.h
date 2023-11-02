//-----------------------------------------------------------------------
// AwesomeVertexCacheManager.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 30/04/2023
//-----------------------------------------------------------------------
// Desc: Interface Class to manage static and dynamic verteces
//----------------------------------------------------------------------

#include <Windows.h>
#include "AwesomeMiscDefinitions.h"
class AwesomeVertexCacheManager
{
public:

	// Constructor/Destructor
	AwesomeVertexCacheManager() {};
	virtual ~AwesomeVertexCacheManager() {};

	// Create a static buffer of vertexes, it returns in the parameter 
	// pnID the index of the buffer you create
	virtual HRESULT CreateStaticBuffer(UINT nSkinID, UINT nVerts, int nVertsStride, const void* pVerts,
		UINT nIndis, const WORD* pIndis, UINT* pnID) = 0;


	// Create a dynamic buffer of vertexes, it returns in the parameter in 
	// pnID the index of the buffer you create, care must be taken as the index 
	// buffer is static and you should keep the memory of the vertex on client side
	virtual HRESULT CreateDynamicBuffer(UINT nSkinID, UINT nVerts, VERTEX* pVerts,
		UINT nIndis, const WORD* pIndis, UINT* pnID) = 0;


	// Update the dynamic buffer vertex data
	virtual HRESULT UpdateDynamicBuffer(UINT nSBufferID, VERTEX* pVerts) = 0;

	// Render static buffers with the default given skin. Index buffers is optional.
	virtual HRESULT Render(UINT nSBufferID) = 0;

};