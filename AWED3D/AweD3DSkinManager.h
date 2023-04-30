//-----------------------------------------------------------------------
// AweD3DException.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 27/04/2023
//-----------------------------------------------------------------------
// Desc: This file contains a class named SkinManager 

#ifndef AWED3D_SKINMANAGER
#define AWED3D_SKINMANAGER

//----------------------------------------------------------------------
// includes
//----------------------------------------------------------------------
#include "../AwesomeRenderer/AwesomeRenderDevice.h"
#include "AweD3DCommandQueue.h"
#include <Windows.h>
#include "d3dx12.h"
#include <DirectXTex.h>
#include <filesystem>
#include <fstream>
#include "AweUtil.h"

//----------------------------------------------------------------------
// Name: AWESOMETEXTURE
// Desc: Texture estructure
//----------------------------------------------------------------------
typedef struct AWETEXTURE_TYPE
{
	float fAlpha;						// Overall transparency value
	std::wstring sName = L"";					// Texture filename
	std::vector<AWESOMECOLOR> pClrKeys;	// Color key array
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;
	D3D12_SHADER_RESOURCE_VIEW_DESC viewDescriptor = {};

} AWESOMETEXTURE;

//----------------------------------------------------------------------
// Name: AWESOMESKIN
// Desc: Skin estructure, it uses a type of material, and up to a 
//		maximum of 8 textures
//----------------------------------------------------------------------
typedef struct AWESKIN_TYPE
{
	bool bAlpha;				// This skin uses transparency?
	UINT nMaterial;				// The ordinal of the material in the array of materials
	std::vector<UINT> nTexture;	// Array of ordinals of texture referencing the array of textures
} AWESOMESKIN;

//----------------------------------------------------------------------
// Name: AweD3DSkinManager (class)
// Desc: Implements a skin manager to avoid materials, textures and
//		skins repeated in the memory
//----------------------------------------------------------------------
class AweD3DSkinManager : public AwesomeSkinManager
{
	//friend class AweD3DVertexCache;
	//friend class AweD3DVertexCacheManager;
	//friend class AweD3D;

public:
	// Constructor/Destructor
	AweD3DSkinManager();
	~AweD3DSkinManager(void);

	void Init(Microsoft::WRL::ComPtr<ID3D12Device2> device, std::shared_ptr<AweD3DCommandQueue> commandQueueShrPtr);

	UINT AddSkin(const AWESOMECOLOR& pcAmbient, const AWESOMECOLOR& pcDiffuse,
		const AWESOMECOLOR& pcEmissive, const AWESOMECOLOR& pcSpecular,
		float fSpecPower);

	HRESULT AddTexture(UINT nSkinID, std::wstring& sName, bool bAlpha,
		float fAlpha);

	bool MaterialEqual(const AWESOMEMATERIAL& pMat0, const AWESOMEMATERIAL& pMat1);

	bool ColorEqual(const AWESOMECOLOR& pCol0, const AWESOMECOLOR& pCol1);


protected:
	AWESOMETEXTURE	CreateTexture(std::wstring fileName);							// Creates a texture

	// TODO:
	/*HRESULT		SetAlphaKey(LPDIRECT3DTEXTURE9* ppTexture, BYTE R, BYTE G,
		BYTE B, BYTE A);								// Set transparency value a espesific color in a texture
	HRESULT		SetTransparency(LPDIRECT3DTEXTURE9* ppTexture, BYTE Alpha);		// Set overall transparency to a texture
	*/
	DWORD		MakeD3DColor(BYTE R, BYTE G, BYTE B, BYTE A);				// Transforms a YCOLOR to a color of D3D

	void		UpdateTexturesDescriptorHeap();

protected:

	Microsoft::WRL::ComPtr<ID3D12Device2> m_pDevice;	// Direct3d device
	std::shared_ptr<AweD3DCommandQueue> m_CommandQueue; // Command queue

	std::vector<AWESOMESKIN>		m_Skins;		// Allocate skins here
	std::vector<AWESOMETEXTURE>		m_Textures;		// Allocate textures here
	std::vector<AWESOMEMATERIAL>	m_Materials;	// Allocate materials here	

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_srvDescriptorHeap;

	UINT m_srvDescriptorSize;

};

#endif // YD3D_SKINMANAGER