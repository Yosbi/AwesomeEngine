//-------------------------------------------------------------------------------
// AwesomeMath.h
// Yosbi Alves
// yosbi@outlook.com
// Copyright (c) 2023
//-----------------------------------------------------------------------
//Desc: In this file is the definition of the clases, data types and methods
//		of the math that the Y-Engine use, it is part of the static math lib
//		implementing basic 3D math objects. I use c++ only in nessesary cases
//		because today every processor supports SSE
//--------------------------------------------------------------------------

#ifndef AWEMATH_H
#define AWEMATH_H

//-----------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------

#include <math.h> 
#include <stdio.h>
#include <memory.h>	//memset

//-------------------------------------------------------------------------
// Constants definitions
//--------------------------------------------------------------------------
const double AWEPI = 3.14159265;
const double AWEPI2 = 1.5707963;
const double AWE2PI = 6.2831853;
const float  AWEEPSILON = 0.00001F;

//-----------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------
class AWEVector;
class AWEMatrix;
class AWEQuat;

//-----------------------------------------------------------------------
// Classes
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
// Name: AWEVector
// Desc: Basic 4d vector class
//-----------------------------------------------------------------------
class __declspec(dllexport) AWEVector
{
public:
    // Costructor and destructor
    AWEVector() { x = 0, y = 0, z = 0, w = 1.0f; }
    AWEVector(float _x, float _y, float _z)
    {
        x = _x; y = _y; z = _z; w = 1.0f;
    }

    // Functions
    inline void		Set(float _x, float _y, float _z, float _w = 1.0f);		// Set the verctor values
    inline float	GetLength();										    // Length (magnitude)
    inline float	GetSqrLength() const;									// Square Length
    inline void		Negate();										        // Vector mult -1
    inline void		Normalize();										    // Normalize vector
    inline float	AngleWith(AWEVector& v);								// Angle in radians
    inline void		RotateWith(const AWEMatrix& m);						    // Apply rotation with matrix
    inline void		InvRotateWith(const AWEMatrix& m);						// Apply inverse rotation with matrix
    inline void		Difference(const AWEVector& v1, const AWEVector& v2);	// Diference from v1 to v2

    void	operator += (const AWEVector& v);								// Operator +=
    void	operator -= (const AWEVector& v);								// Operator -=
    void	operator *= (float f);											// Scalar mult (scale vector)
    void	operator /= (float f);											// Scale down
    void	operator += (float f);											// Add scalar
    void	operator -= (float f);											// Substract scalar
    float	operator *  (const AWEVector& v) const;							// Dot product
    AWEVector operator *  (float f)		   const;							// Scale vector
    AWEVector operator /  (float f)		   const;							// Scalar divide
    AWEVector operator +  (float f)		   const;							// Add scalar
    AWEVector operator -  (float f)		   const;							// Substract scalar
    AWEVector operator *  (const AWEMatrix& m) const;						// Vector-matrix product
    AWEVector operator +  (const AWEVector& v) const;						// Vector addition
    AWEVector operator -  (const AWEVector& v) const;						// Substraction

    inline AWEVector Cross(const AWEVector& v2);						    // Cross product

    // Variables
    float x, y, z, w;		// Coordinate set

};

//-----------------------------------------------------------------------
// Name: YMatrix
// Desc: Basic matrix class
//-----------------------------------------------------------------------
class __declspec(dllexport) AWEMatrix {
public:

    // Constructor
    AWEMatrix(void);

    // Functions
    inline void Identity(void);										    // identity matrix
    inline void RotaX(float a);										    // Rotation matrix arround x axis
    inline void RotaY(float a);										    // Rotation matrix arround y axis
    inline void RotaZ(float a);										    // Rotation matrix arround z axis
    inline void Rota(const AWEVector& vc);								// Rotation matrix arround x, y and z
    inline void Rota(float x, float y, float z);						// Rotation matrix arround x, y and z
    inline void RotaArbi(const AWEVector& vcAxis, float a);				// Rotation arround an arbitrary axis
    inline void ApplyInverseRota(AWEVector* pvc);						// Inverse rotation
    inline void Translate(float dx, float dy, float dz);				// Translate a certain distance
    inline void SetTranslation(AWEVector vc, bool EraseContent = false);// Set translation values
    inline AWEVector GetTranslation(void);								// Get the translation value
    inline void Scale(float dx, float dy, float dz);                    // Make a scale matrix to the d factors


    inline void LookAt(AWEVector vcPos, AWEVector vcLookAt,
        AWEVector vcWorldUp = AWEVector(0, 1, 0));

    inline void TransposeOf(const AWEMatrix& m);						// transpose m, save result in this
    inline void InverseOf(const AWEMatrix& m);							// invert m, save result in this	  
   
    AWEMatrix operator * (const AWEMatrix& m)const;						// matrix multiplication
    AWEVector operator * (const AWEVector& vc)const;					// matrix vector multiplication

    // Variables
    float _11, _12, _13, _14;
    float _21, _22, _23, _24;
    float _31, _32, _33, _34;
    float _41, _42, _43, _44;
};

//-----------------------------------------------------------------------
// Name: AWEOPolygon
// Desc: Basic quaternion class
//-----------------------------------------------------------------------
class __declspec(dllexport) AWEQuat {
public:
    // Constructors
    AWEQuat(void) { x = 0.0f, y = 0.0f, z = 0.0f, w = 1.0f; }
    AWEQuat(float _x, float _y, float _z, float _w)
    {
        x = _x; y = _y; z = _z; w = _w;
    }

    // Functions
    void  MakeFromEuler(float fPitch, float fYaw, float fRoll);
    void  Normalize();
    void  Conjugate(AWEQuat q);
    void  GetEulers(float* fPitch, float* fYaw, float* fRoll);
    void  GetMatrix(AWEMatrix* m);
    float GetMagnitude(void);


    void  operator /= (float f);
    AWEQuat operator /  (float f);

    void  operator *= (float f);
    AWEQuat operator *  (float f);

    AWEQuat operator *  (const AWEVector& v) const;

    AWEQuat operator *  (const AWEQuat& q) const;
    void  operator *= (const AWEQuat& q);

    void  operator += (const AWEQuat& q);
    AWEQuat operator +  (const AWEQuat& q) const;

    AWEQuat operator~(void) const { return AWEQuat(-x, -y, -z, w); }

    void  Rotate(const AWEQuat& q1, const AWEQuat& q2);

    AWEVector Rotate(const AWEVector& v);

    // Variables
    float x, y, z, w;
};

#endif //AWEMATH_H