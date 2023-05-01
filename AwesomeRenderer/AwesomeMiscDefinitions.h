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

//-----------------------------------------------------------------------
// Vertex types
//-----------------------------------------------------------------------
//----------------------------------------------------------------------
// Name: YVERTEXID
// Desc: Definition of the vertex types in a enum
//----------------------------------------------------------------------
typedef enum AWEVERTEXID_TYPE
{
	VID_PS,		// Untransformed position only
	VID_UU,		// Untransformed and unlit vertex
	VID_UL,		// Untransformed and lit vertex
	//VID_CA,		// Used for character animation
	//VID_3T,		// Three texture coords pairs
	//VID_TV		// Like UU but with tangent vector
} AWESOMEVERTEXID;

// Just untransformed position
typedef struct PVERTEX_TYPE {
	float x, y, z;
} PVERTEX;

// Untransformed unlit vertex
typedef struct VERTEX_TYPE {
	float x, y, z;
	float vcN[3];
	float tu, tv;
} VERTEX;

// Untransformed and lit vertex
typedef struct LVERTEX_TYPE {
	float x, y, z;
	DWORD Color;
	float tu, tv;
} LVERTEX;
#endif
