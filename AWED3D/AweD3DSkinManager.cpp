//-------------------------------------------------------------------------------
// AweD3DSkinManager.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 27/04/2023
//-----------------------------------------------------------------------
//Desc: Implementation of the AweD3DSkinManager

//----------------------------------------------------------------------
// includes
//----------------------------------------------------------------------
#include "AweD3DSkinManager.h" // Class definition


//-----------------------------------------------------------------------------
// Name: Constructor
// Desc: Initializes all the variables	
//-----------------------------------------------------------------------------
AweD3DSkinManager::AweD3DSkinManager()
{
}

//-----------------------------------------------------------------------------
// Name: Destructor
// Desc: Realease direct3d texture objects
//-----------------------------------------------------------------------------
AweD3DSkinManager::~AweD3DSkinManager()
{
}

void AweD3DSkinManager::Init(Microsoft::WRL::ComPtr<ID3D12Device2> device, std::shared_ptr<AweD3DCommandQueue> commandQueueShrPtr)
{
	m_pDevice = device;
	m_CommandQueue = commandQueueShrPtr;
	m_nSrvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

}

//-----------------------------------------------------------------------------
// Name: AddSkin
// Desc: Creates a new skin object using the given material. Texture is set
//		to NULL till a texture is added to that skin.
//		The var npSkinID is an out var, a id to reference the new skin
//-----------------------------------------------------------------------------
UINT AweD3DSkinManager::AddSkin(const AWESOMECOLOR& pcAmbient, const AWESOMECOLOR& pcDiffuse,
	const AWESOMECOLOR& pcEmissive, const AWESOMECOLOR& pcSpecular,
	float fSpecPower)
{
	UINT	  nMat, n;
	bool	  bMat = false; // To know if the material is already in memory
	AWESOMEMATERIAL mat;	// The new material
	AWESOMESKIN skin;		// The new skin

	mat.cAmbient = pcAmbient;
	mat.cDiffuse = pcDiffuse;
	mat.cEmissive = pcEmissive;
	mat.cSpecular = pcSpecular;
	mat.fPower = fSpecPower;

	// Check if already have a material in memory
	for (nMat = 0; nMat < m_Materials.size(); nMat++)
	{
		if (MaterialEqual(mat, m_Materials[nMat]))
		{
			bMat = true;
			break;
		}
	}

	if (!bMat)
	{
		// TODO: make this thread safe in the future
		nMat = m_Materials.size();
		m_Materials.push_back(mat);
	}

	skin.nMaterial = nMat;

	// Set alpha value if needed
	if (mat.cDiffuse.fA < 1.0f)
		skin.bAlpha = true;
	else
		skin.bAlpha = false;


	// Saving the skin
	// TODO: make this thread safe in the future
	int skinID = m_Skins.size();
	m_Skins.push_back(skin);

	return skinID;
}


//-----------------------------------------------------------------------------
// Name: AddTexture
// Desc: Add a texture to a given skin. BMP is only loaded if not used yet
//		can alse be used to set alpha channels on textures for alpha keys and/or
//		overall transparency. Set bool to true in both cases
//-----------------------------------------------------------------------------
HRESULT AweD3DSkinManager::AddTexture(UINT nSkinID, std::wstring& sName, bool bAlpha,
	float fAlpha)
{
	AWESOMETEXTURE* pYTex = NULL;
	HRESULT	  hr;
	UINT	  nTex, n;
	bool	  bTex = false; // To know if the texture has been introduced before

	std::wstring fileName = GetAssetFullPath(sName.c_str());


	// If this is a valid skin 
	if (nSkinID >= m_Skins.size()) return E_INVALIDARG;

	// TODO: validate the number of textures on this skin

	// Is the texture already loaded in memory?
	for (nTex = 0; nTex < m_Textures.size(); nTex++)
	{
		if (fileName == m_Textures[nTex].sName)
		{
			bTex = true;
			break;
		}
	}

	// Setting the alpha blending to the skin if the texture has alpha blending
	if (bAlpha)
		m_Skins[nSkinID].bAlpha = bAlpha;


	// If the texture is not loaded
	if (!bTex)
	{
		// Create d3d texture from that pointer
		AWESOMETEXTURE texture = CreateTexture(fileName);

		// If it not use alphablending (No alphablending for now)
		texture.fAlpha = 1.0;

		// TODO: multithreading
		// Save id and add to count
		nTex = m_Textures.size();
		m_Textures.push_back(texture);
	}// if not texture

	// Puting texture id into the skin reference to a texture array
	m_Skins[nSkinID].nTexture.push_back(nTex);
	
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: ColorEqual
// Desc: Compares two YCOLORS and returns 1 if equal
//-----------------------------------------------------------------------------
inline bool AweD3DSkinManager::ColorEqual(const AWESOMECOLOR& pCol0, const AWESOMECOLOR& pCol1)
{
	if ((pCol0.fA == pCol1.fA) && (pCol0.fB == pCol1.fB) && (pCol0.fG == pCol1.fG) && (pCol0.fR == pCol1.fR))
		return true;
	return false;
}

//-----------------------------------------------------------------------------
// Name: MaterialEqual
// Desc: Compares two YMATERIALs and returns 1 if equal
//-----------------------------------------------------------------------------
bool AweD3DSkinManager::MaterialEqual(const AWESOMEMATERIAL& pMat0, const AWESOMEMATERIAL& pMat1)
{
	if (ColorEqual(pMat0.cAmbient, pMat1.cAmbient) && ColorEqual(pMat0.cDiffuse, pMat1.cDiffuse) &&
		ColorEqual(pMat0.cEmissive, pMat1.cEmissive) && ColorEqual(pMat0.cSpecular, pMat1.cSpecular) &&
		(pMat0.fPower == pMat1.fPower))
		return true;
	return false;
}

//-----------------------------------------------------------------------------
// Name: CreateTexture
// Desc: Creates a D3DTexture object and loads the image data from disc
//-----------------------------------------------------------------------------
AWESOMETEXTURE AweD3DSkinManager::CreateTexture(std::wstring fileName)
{
	// Verifing if the file exist
	std::filesystem::path filePath(fileName);
	if (!std::filesystem::exists(filePath))
	{
		throw std::exception("File not found.");
	}

	AWESOMETEXTURE aweTexture;
	aweTexture.sName = fileName;

	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratchImage;

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList = m_CommandQueue->GetCommandList();

	if (filePath.extension() == ".dds")
	{
		ThrowIfFailed(LoadFromDDSFile(
			fileName.c_str(),
			DirectX::DDS_FLAGS_FORCE_RGB,
			&metadata,
			scratchImage));
	}
	else if (filePath.extension() == ".hdr")
	{
		ThrowIfFailed(LoadFromHDRFile(
			fileName.c_str(),
			&metadata,
			scratchImage));
	}
	else if (filePath.extension() == ".tga")
	{
		ThrowIfFailed(LoadFromTGAFile(
			fileName.c_str(),
			&metadata,
			scratchImage));
	}
	else
	{
		ThrowIfFailed(LoadFromWICFile(
			fileName.c_str(),
			DirectX::WIC_FLAGS_FORCE_RGB,
			&metadata,
			scratchImage));
	}

	D3D12_RESOURCE_DESC textureDesc = {};
	switch (metadata.dimension)
	{
	case DirectX::TEX_DIMENSION_TEXTURE1D:
		textureDesc = CD3DX12_RESOURCE_DESC::Tex1D(
			metadata.format,
			static_cast<UINT64>(metadata.width),
			static_cast<UINT16>(metadata.arraySize));
		break;
	case DirectX::TEX_DIMENSION_TEXTURE2D:
		textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			metadata.format,
			static_cast<UINT64>(metadata.width),
			static_cast<UINT>(metadata.height),
			static_cast<UINT16>(metadata.arraySize));
		break;
	case DirectX::TEX_DIMENSION_TEXTURE3D:
		textureDesc = CD3DX12_RESOURCE_DESC::Tex3D(
			metadata.format,
			static_cast<UINT64>(metadata.width),
			static_cast<UINT>(metadata.height),
			static_cast<UINT16>(metadata.depth));
		break;
	default:
		throw std::exception("Invalid texture dimension.");
		break;
	}
	textureDesc.Flags |= D3D12_RESOURCE_FLAG_NONE;

	// Creating texture resource
	m_pDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), 
		D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_COPY_DEST, 
		nullptr, IID_PPV_ARGS(&aweTexture.textureResource));

	// Creating the upload heap 
	Microsoft::WRL::ComPtr<ID3D12Resource> textureUploadHeap;
	m_pDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(scratchImage.GetPixelsSize()),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&textureUploadHeap));

	// And copy the data to the texture resource
	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData = scratchImage.GetPixels();
	textureData.RowPitch = scratchImage.GetImages()->rowPitch;
	textureData.SlicePitch = scratchImage.GetPixelsSize();
	UpdateSubresources(commandList.Get(), aweTexture.textureResource.Get(), textureUploadHeap.Get(), 0, 0, metadata.mipLevels, &textureData);

	// transition the texture resource to the shader resource state
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(aweTexture.textureResource.Get(), 
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandList->ResourceBarrier(1, &barrier);

	// Resize the descriptor heap
	UpdateTexturesDescriptorHeap();

	// create SRV descriptor for texture resource
	aweTexture.viewDescriptor.Format = textureDesc.Format;
	aweTexture.viewDescriptor.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	aweTexture.viewDescriptor.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	aweTexture.viewDescriptor.Texture2D.MipLevels = 1;

	// Looking for the right place in the heap
	CD3DX12_CPU_DESCRIPTOR_HANDLE srvHeapHandle(m_srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	srvHeapHandle.Offset(m_Textures.size(), m_nSrvDescriptorSize);

	// Saving in the heap
	m_pDevice->CreateShaderResourceView(aweTexture.textureResource.Get(), &aweTexture.viewDescriptor, m_srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	
	m_CommandQueue->ExecuteCommandList(commandList);
	return aweTexture;
}

//-----------------------------------------------------------------------------
// Name: SetAlphaKey
// Desc: Sets the alpha channel of all pixels with 
//		given RGB value to the amount of transparency especified, Make 
//		sure the corresponding RGB values in the texture have alpha value 
//		set to 1.0f, so set all alpha key prior to calling SetTransparency. 
//		The range ofvalues of RGBA vars is 0 to 255
//-----------------------------------------------------------------------------
/*HRESULT AweD3DSkinManager::SetAlphaKey(LPDIRECT3DTEXTURE9* ppTexture, BYTE R,
	BYTE G, BYTE B, BYTE A)
{
	D3DSURFACE_DESC d3dDesc; // Descryption of the surface
	D3DLOCKED_RECT	d3dRect;
	DWORD			dwKey, Color;

	// Security check: Must be ARGB format
	(*ppTexture)->GetLevelDesc(0, &d3dDesc);
	if (d3dDesc.Format != D3DFMT_A8R8G8B8)
		return Y_INVALIDPARAM;

	// Calculate the key value
	dwKey = MakeD3DColor(R, G, B, 255);

	// Calculate new color tto set for key pixels
	if (A > 0) Color = MakeD3DColor(R, G, B, A);
	else	  Color = MakeD3DColor(0, 0, 0, A);

	if (FAILED((*ppTexture)->LockRect(0, &d3dRect, NULL, 0)))
		return Y_BUFFERLOCK;

	// Overwrite all key pixels with new color valur
	for (DWORD y = 0; y < d3dDesc.Height; y++)
	{
		for (DWORD x = 0; x < d3dDesc.Width; x++)
		{
			if (((DWORD*)d3dRect.pBits)[d3dDesc.Width * y + x] == dwKey)
				((DWORD*)d3dRect.pBits)[d3dDesc.Width * y + x] = Color;
		}
	}

	(*ppTexture)->UnlockRect(0);

	return Y_OK;
}*/

//-----------------------------------------------------------------------------
// Name:MakeD3DColor
// Desc: Calculates a 32 bit ARGB value for use with d3d for a given RGBA
//		 color values rangin from 0 to 255
//-----------------------------------------------------------------------------
DWORD AweD3DSkinManager::MakeD3DColor(BYTE R, BYTE G, BYTE B, BYTE A)
{
	return (A << 24) | (R << 16) | (G << 8) | B;
}

void AweD3DSkinManager::UpdateTexturesDescriptorHeap()
{
	// TODO: Maybe instead of one for one we must duplicate the size

	UINT oldNumDescriptors = m_Textures.size();
	UINT newNumDescriptors = m_Textures.size() + 1;

	// Create a new descriptor heap for SRVs with the desired number of descriptors
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = newNumDescriptors;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> newHeap;
	ThrowIfFailed(m_pDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&newHeap)));

	// Copy the existing descriptors from the old heap to the new one
	if (oldNumDescriptors > 0) 
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE srcHandle(m_srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		CD3DX12_CPU_DESCRIPTOR_HANDLE dstHandle(newHeap->GetCPUDescriptorHandleForHeapStart());

		m_pDevice->CopyDescriptorsSimple(oldNumDescriptors, newHeap->GetCPUDescriptorHandleForHeapStart(),
			srcHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	// Replace the old descriptor heap with the new one
	m_srvDescriptorHeap = newHeap;
}

//-----------------------------------------------------------------------------
// Name: SetTransparency
// Desc: Sets all pixels of the texture to the amount of transparency
//		 (overall transparency)
//-----------------------------------------------------------------------------
/*HRESULT AweD3DSkinManager::SetTransparency(LPDIRECT3DTEXTURE9* ppTexture, UCHAR Alpha)
{
	D3DSURFACE_DESC	d3dDesc;
	D3DLOCKED_RECT	d3dRect;
	DWORD			Color;
	WORD			A, R, G, B;

	(*ppTexture)->GetLevelDesc(0, &d3dDesc); // Obtain the description of the texture loaded

	// Must be an ARGB format
	if (d3dDesc.Format != D3DFMT_A8R8G8B8)
		return Y_INVALIDPARAM;

	if (FAILED((*ppTexture)->LockRect(0, &d3dRect, NULL, 0)))
		return Y_BUFFERLOCK;

	// Loop through all pixels
	for (DWORD y = 0; y < d3dDesc.Height; y++)
	{
		for (DWORD x = 0; x < d3dDesc.Width; x++)
		{
			// Get color from this pixel
			Color = ((DWORD*)d3dRect.pBits)[d3dDesc.Width * y + x];

			// Calculate ARGB values from pixel color
			A = (UCHAR)((Color & 0xff000000) >> 24);
			R = (UCHAR)((Color & 0x00ff0000) >> 16);
			G = (UCHAR)((Color & 0x0000ff00) >> 8);
			B = (UCHAR)((Color & 0x000000ff) >> 0);

			// Only set new alpha value if old value is greater
			if (A >= Alpha)
				A = Alpha;

			// Putting new color value for this pixel
			((DWORD*)d3dRect.pBits)[d3dDesc.Width * y + x] = MakeD3DColor(R, G, B, A);
		}
	}
	(*ppTexture)->UnlockRect(0);
	return Y_OK;
}*/
