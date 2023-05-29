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
#include "../AwesomeMath/AwesomeMath.h"

#define MAX_LIGHTS 128

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

/*typedef struct AWEMATERIAL_TYPE
{
	AWESOMECOLOR cDiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };  // RGBA diffuse light
	AWESOMECOLOR FresnelR0 = { 0.01f, 0.01f, 0.01f };
	float Roughness = 0.25f;
} AWESOMEMATERIAL;*/

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
	VERTEX_TYPE(){}
	VERTEX_TYPE(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float u, float v) :
		x(px), y(py), z(pz),
		tu(u), tv(v)
	{
		vcN[0] = nx; vcN[1] = ny; vcN[2] = nz;
	};
	
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

//----------------------------------------------------------------------
// Name: AWELIGHTID
// Desc: Definition of the types of light
//----------------------------------------------------------------------
typedef enum AWELIGHTID_TYPE
{
	AWE_NOTSET_LIGHT = 0,		// Light not set
	AWE_DIRECTIONAL_LIGHT = 1,  // Directional light source
	AWE_POINT_LIGHT = 2,        // Point light source
	AWE_SPOT_LIGHT = 3,         // Spot light source

} AWELIGHTID;

//----------------------------------------------------------------------
// Name: AWELIGHT
// Desc: Definition of a light
//----------------------------------------------------------------------
typedef struct AWELIGHT_TYPE
{

	AWESOMECOLOR cDiffuseLight = { 1.0f, 1.0f, 1.0f, 1.0f };    // Diffuse light color
	AWESOMECOLOR cSpecularLight = { 0.7f, 0.7f, 0.7f, 1.0f };   // Specular light color

	AWEVector    vcPositionW = AWEVector(0.0f, 0.0f, 0.0f);		// Diffuse light vector
	AWEVector	 vcDirectionW = AWEVector(0.0f, 0.0f, 0.0f);	// Light direction

	float      fRange = 0.0f;         // Range of light
	float      fTheta = 0.0f;         // Angle of spot light inner cone
	float      fPhi = 0.0f;           // Angle of spot light outer cone
	float      fAttenuation0 = 0.0f;  // Change of intensity over distance
	float      fAttenuation1 = 0.0f;  // Change of intensity over distance
	float	   fAttenuation2 = 0.0f;  // Change of intensity over distance

	AWELIGHTID   Type = AWE_NOTSET_LIGHT; // Type of light, not set by default
	float notUsed = 0.0f;

} AWELIGHT;

typedef struct AWEPERPASSVARIABLES_TYPE
{
	AWEMatrix mView;
	AWEMatrix mInvView;
	AWEMatrix mProj;
	AWEMatrix mInvProj;
	AWEMatrix mViewProj;
	AWEMatrix mInvViewProj;
	AWEVector vcEyePosW;
	AWESOMECOLOR cAmbientLight = { 0.6f, 0.6f, 0.6f, 1.0f };	// Ambient light color
	AWELIGHT light[MAX_LIGHTS];
	float time; // delta time passed 
}
AWEPASSVARIABLES;

typedef struct AWEPEROBJECTVARIABLES_TYPE
{
	AWEMatrix	 mWorld;											// World view proj combo matrix;
	AWEMatrix	 mWorldInverseTranspose;						// World matrix inversed and transposed
	AWESOMEMATERIAL material;
}
AWEPEROBJECTVARIABLES;
#endif