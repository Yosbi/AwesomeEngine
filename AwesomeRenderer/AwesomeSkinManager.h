//-----------------------------------------------------------------------
// AwesomeSkinManager.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 27/04/2023
//-----------------------------------------------------------------------
// Desc: AwesomeSkinManager is an abstract class which provides an interface
//		from which material managers can inherit. Those heirs need to 
//		implement a store for texture data in a way appropriate to the
//		API.

#ifndef AWESOMESKINMAN_H
#define AWESOMESKINMAN_H
#include <Windows.h>
#include "AwesomeMiscDefinitions.h"

class AwesomeSkinManager
{
public:

	// Constructor/Destructor
	AwesomeSkinManager() {};
	virtual ~AwesomeSkinManager() {};

	// Add a skin to the skin manager, you have to provide 
	// the id where the skin must be saved
	virtual UINT AddSkin(const AWESOMECOLOR& pcAmbient, const AWESOMECOLOR& pcDiffuse,
		const AWESOMECOLOR& pcEmissive, const AWESOMECOLOR& pcSpecular,
		float fSpecPower) = 0;

	// Add a texture to a skin
	virtual HRESULT AddTexture(UINT nSkinID, std::wstring& sName, bool bAlpha,
		float fAlpha) = 0;

	// Compares to materials and returns 1 if equal and 0 if not
	virtual bool	MaterialEqual(const AWESOMEMATERIAL& pMat0, const AWESOMEMATERIAL& pMat1) = 0;

	// Compares two color and returns 1 if equal
	virtual	bool	ColorEqual(const AWESOMECOLOR& pCol0, const AWESOMECOLOR& pCol1) = 0;

};
typedef class AwesomeSkinManager* LPAWESKINMANAGER;
#endif