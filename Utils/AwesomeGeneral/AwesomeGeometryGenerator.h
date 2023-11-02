//-----------------------------------------------------------------------
// AwesomeGeomtryGenerator.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 07/05/2023
//-----------------------------------------------------------------------
// Desc: Basic geometry generator, inspired by Frank Luna

#ifndef AWEGEOMETRYGEN_H
#define AWEGEOMETRYGEN_H
#pragma once

#include <cstdint>
#include <DirectXMath.h>
#include <vector>

class AwesomeGeometryGenerator
{
public:

	using uint16 = std::uint16_t;
	using uint32 = std::uint32_t;
	using uint64 = std::uint64_t;

	struct AwesomeMeshData
	{
		std::vector<VERTEX> Vertices;
		std::vector<uint32> Indices32;

		uint32 getVertexSize() { return sizeof(VERTEX); }
		std::vector<uint16>& GetIndices16()
		{
			if (mIndices16.empty())
			{
				mIndices16.resize(Indices32.size());
				for (size_t i = 0; i < Indices32.size(); ++i)
					mIndices16[i] = static_cast<uint16>(Indices32[i]);
			}

			return mIndices16;
		}

	private:
		std::vector<uint16> mIndices16;
	};

	// Creates a box centered at the origin with the given dimensions, where each
	// face has m rows and n columns of vertices.
	AwesomeMeshData CreateBox(float width, float height, float depth, uint32 numSubdivisions);

	// Creates a sphere centered at the origin with the given radius.  The
	// slices and stacks parameters control the degree of tessellation.
	AwesomeMeshData CreateSphere(float radius, uint32 sliceCount, uint32 stackCount);

	// Creates a geosphere centered at the origin with the given radius.  The
	// depth controls the level of tessellation.
	AwesomeMeshData CreateGeosphere(float radius, uint32 numSubdivisions);

	// Creates a cylinder parallel to the y-axis, and centered about the origin.  
	// The bottom and top radius can vary to form various cone shapes rather than true
	// cylinders.  The slices and stacks parameters control the degree of tessellation.
	AwesomeMeshData CreateCylinder(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount);

	// Creates an mxn grid in the xz-plane with m rows and n columns, centered
	// at the origin with the specified width and depth.
	AwesomeMeshData CreateGrid(float width, float depth, uint64_t m, uint64_t n);

	// Creates a quad aligned with the screen.  This is useful for postprocessing and screen effects.
	AwesomeMeshData CreateQuad(float x, float y, float w, float h, float depth);

private:
	void Subdivide(AwesomeMeshData& meshData);
	VERTEX MidPoint(const VERTEX& v0, const VERTEX& v1);
	void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, AwesomeMeshData& meshData);
	void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, AwesomeMeshData& meshData);
};



#endif // !AWEGEOMETRYGEN_H

