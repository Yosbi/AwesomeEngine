//-----------------------------------------------------------------------
// AwesomeMiscDefinitions.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 17/04/2023
//-----------------------------------------------------------------------
// Desc: This file contain a set of definitions that are used in all the
//	     engine
#ifndef AWESOMEMISC_H
#define AWESOMEMISC_H
#pragma once

#include <Windows.h>

//----------------------------------------------------------------------
// Name: AWESOMECOLOR
// Desc: Basic color structure
//----------------------------------------------------------------------
typedef struct AWECOLOR_TYPE
{
	union
	{
		struct
		{
			float fR;
			float fG;
			float fB;
			float fA;
		};
		float c[4];
	};
} AWESOMECOLOR;


//----------------------------------------------------------------------
// Name: AWESOMEMATERIAL
// Desc: Material structure
//----------------------------------------------------------------------
typedef struct AWEMATERIAL_TYPE
{
	AWESOMECOLOR cDiffuse;  // RGBA diffuse light
	AWESOMECOLOR cAmbient;  // RGBA ambient light
	AWESOMECOLOR cSpecular; // RGBA specular light
	AWESOMECOLOR cEmissive; // RGBA emissive light
	float  fPower;			// Specular power
} AWESOMEMATERIAL;

#endif
