//-----------------------------------------------------------------------
// AwesomeGeomtryGenerator.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 07/05/2023
//-----------------------------------------------------------------------
// Desc: Implementation of the basic geometry generator, 
//	inspired by Frank Luna
#include "AwesomeGeneral.h"
#include <algorithm>

using namespace DirectX;

AwesomeGeometryGenerator::AwesomeMeshData AwesomeGeometryGenerator::CreateBox(float width, float height, float depth, uint32 numSubdivisions)
{
	AwesomeMeshData meshData;

	//
	// Create the vertices.
	//

	VERTEX v[24];

	float w2 = 0.5f * width;
	float h2 = 0.5f * height;
	float d2 = 0.5f * depth;

	// Fill in the front face vertex data.
	v[0] = VERTEX(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[1] = VERTEX(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[2] = VERTEX(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[3] = VERTEX(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Fill in the back face vertex data.
	v[4] = VERTEX(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	v[5] = VERTEX(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[6] = VERTEX(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[7] = VERTEX(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);

	// Fill in the top face vertex data.
	v[8] = VERTEX(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[9] = VERTEX(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[10] = VERTEX(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[11] = VERTEX(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the bottom face vertex data.
	v[12] = VERTEX(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);
	v[13] = VERTEX(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	v[14] = VERTEX(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
	v[15] = VERTEX(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the left face vertex data.
	v[16] = VERTEX(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[17] = VERTEX(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[18] = VERTEX(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[19] = VERTEX(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the right face vertex data.
	v[20] = VERTEX(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[21] = VERTEX(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[22] = VERTEX(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[23] = VERTEX(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	meshData.Vertices.assign(&v[0], &v[24]);

	//
	// Create the indices.
	//

	uint32 i[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	meshData.Indices32.assign(&i[0], &i[36]);

	// Put a cap on the number of subdivisions.
	numSubdivisions = std::min<uint32>(numSubdivisions, 6u);

	for (uint32 i = 0; i < numSubdivisions; ++i)
		Subdivide(meshData);

	return meshData;
}

AwesomeGeometryGenerator::AwesomeMeshData AwesomeGeometryGenerator::CreateSphere(float radius, uint32 sliceCount, uint32 stackCount)
{
	AwesomeMeshData meshData;

	//
	// Compute the vertices stating at the top pole and moving down the stacks.
	//

	// Poles: note that there will be texture coordinate distortion as there is
	// not a unique point on the texture map to assign to the pole when mapping
	// a rectangular texture onto a sphere.
	VERTEX topVertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, 0.0f);
	VERTEX bottomVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);

	meshData.Vertices.push_back(topVertex);

	float phiStep = XM_PI / stackCount;
	float thetaStep = 2.0f * XM_PI / sliceCount;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (uint32 i = 1; i <= stackCount - 1; ++i)
	{
		float phi = i * phiStep;

		// Vertices of ring.
		for (uint32 j = 0; j <= sliceCount; ++j)
		{
			float theta = j * thetaStep;

			VERTEX v;

			// spherical to cartesian
			v.x = radius * sinf(phi) * cosf(theta);
			v.y = radius * cosf(phi);
			v.z = radius * sinf(phi) * sinf(theta);

			AWEVector p(v.x, v.y, v.z);
			p.Normalize();
			v.vcN[0] = p.x; v.vcN[1] = p.y; v.vcN[2] = p.z;

			v.tu = theta / XM_2PI;
			v.tv = phi / XM_PI;

			meshData.Vertices.push_back(v);
		}
	}

	meshData.Vertices.push_back(bottomVertex);

	//
	// Compute indices for top stack.  The top stack was written first to the vertex buffer
	// and connects the top pole to the first ring.
	//

	for (uint32 i = 1; i <= sliceCount; ++i)
	{
		meshData.Indices32.push_back(0);
		meshData.Indices32.push_back(i + 1);
		meshData.Indices32.push_back(i);
	}

	//
	// Compute indices for inner stacks (not connected to poles).
	//

	// Offset the indices to the index of the first vertex in the first ring.
	// This is just skipping the top pole vertex.
	uint32 baseIndex = 1;
	uint32 ringVertexCount = sliceCount + 1;
	for (uint32 i = 0; i < stackCount - 2; ++i)
	{
		for (uint32 j = 0; j < sliceCount; ++j)
		{
			meshData.Indices32.push_back(baseIndex + i * ringVertexCount + j);
			meshData.Indices32.push_back(baseIndex + i * ringVertexCount + j + 1);
			meshData.Indices32.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			meshData.Indices32.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			meshData.Indices32.push_back(baseIndex + i * ringVertexCount + j + 1);
			meshData.Indices32.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	//
	// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	// and connects the bottom pole to the bottom ring.
	//

	// South pole vertex was added last.
	uint32 southPoleIndex = (uint32)meshData.Vertices.size() - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;

	for (uint32 i = 0; i < sliceCount; ++i)
	{
		meshData.Indices32.push_back(southPoleIndex);
		meshData.Indices32.push_back(baseIndex + i);
		meshData.Indices32.push_back(baseIndex + i + 1);
	}

	return meshData;
}

void AwesomeGeometryGenerator::Subdivide(AwesomeMeshData& meshData)
{
	// Save a copy of the input geometry.
	AwesomeMeshData inputCopy = meshData;


	meshData.Vertices.resize(0);
	meshData.Indices32.resize(0);

	//       v1
	//       *
	//      / \
	//     /   \
	//  m0*-----*m1
	//   / \   / \
	//  /   \ /   \
	// *-----*-----*
	// v0    m2     v2

	uint32 numTris = (uint32)inputCopy.Indices32.size() / 3;
	for (uint32 i = 0; i < numTris; ++i)
	{
		VERTEX v0 = inputCopy.Vertices[inputCopy.Indices32[i * 3 + 0]];
		VERTEX v1 = inputCopy.Vertices[inputCopy.Indices32[i * 3 + 1]];
		VERTEX v2 = inputCopy.Vertices[inputCopy.Indices32[i * 3 + 2]];

		//
		// Generate the midpoints.
		//

		VERTEX m0 = MidPoint(v0, v1);
		VERTEX m1 = MidPoint(v1, v2);
		VERTEX m2 = MidPoint(v0, v2);

		//
		// Add new geometry.
		//

		meshData.Vertices.push_back(v0); // 0
		meshData.Vertices.push_back(v1); // 1
		meshData.Vertices.push_back(v2); // 2
		meshData.Vertices.push_back(m0); // 3
		meshData.Vertices.push_back(m1); // 4
		meshData.Vertices.push_back(m2); // 5

		meshData.Indices32.push_back(i * 6 + 0);
		meshData.Indices32.push_back(i * 6 + 3);
		meshData.Indices32.push_back(i * 6 + 5);

		meshData.Indices32.push_back(i * 6 + 3);
		meshData.Indices32.push_back(i * 6 + 4);
		meshData.Indices32.push_back(i * 6 + 5);

		meshData.Indices32.push_back(i * 6 + 5);
		meshData.Indices32.push_back(i * 6 + 4);
		meshData.Indices32.push_back(i * 6 + 2);

		meshData.Indices32.push_back(i * 6 + 3);
		meshData.Indices32.push_back(i * 6 + 1);
		meshData.Indices32.push_back(i * 6 + 4);
	}
}

VERTEX AwesomeGeometryGenerator::MidPoint(const VERTEX& v0, const VERTEX& v1)
{
	AWEVector p0(v0.x, v0.y, v0.z);
	AWEVector p1(v1.x, v1.y, v1.z);

	AWEVector n0(v0.vcN[0], v0.vcN[1], v0.vcN[2]);
	AWEVector n1(v1.vcN[0], v1.vcN[1], v1.vcN[2]);

	AWEVector tex0(v0.tu, v0.tv, 0.0f);
	AWEVector tex1(v1.tu, v1.tv, 0.0f);

	// Compute the midpoints of all the attributes.  Vectors need to be normalized
	// since linear interpolating can make them not unit length.  
	AWEVector pos = (p0 + p1) * 0.5f;
	AWEVector normal((n0 + n1) * 0.5f); normal.Normalize();
	AWEVector tex = (tex0 + tex1) * 0.5f;

	VERTEX v(pos.x, pos.y, pos.z, normal.x, normal.y, normal.z, tex.x, tex.y);

	return v;
}

AwesomeGeometryGenerator::AwesomeMeshData AwesomeGeometryGenerator::CreateGeosphere(float radius, uint32 numSubdivisions)
{
	AwesomeMeshData meshData;

	// Put a cap on the number of subdivisions.
	numSubdivisions = std::min<uint32>(numSubdivisions, 6u);

	// Approximate a sphere by tessellating an icosahedron.

	const float X = 0.525731f;
	const float Z = 0.850651f;

	AWEVector pos[12] =
	{
		AWEVector(-X, 0.0f, Z),  AWEVector(X, 0.0f, Z),
		AWEVector(-X, 0.0f, -Z), AWEVector(X, 0.0f, -Z),
		AWEVector(0.0f, Z, X),   AWEVector(0.0f, Z, -X),
		AWEVector(0.0f, -Z, X),  AWEVector(0.0f, -Z, -X),
		AWEVector(Z, X, 0.0f),   AWEVector(-Z, X, 0.0f),
		AWEVector(Z, -X, 0.0f),  AWEVector(-Z, -X, 0.0f)
	};

	uint32 k[60] =
	{
		1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
		1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
	};

	meshData.Vertices.resize(12);
	meshData.Indices32.assign(&k[0], &k[60]);

	for (uint32 i = 0; i < 12; ++i) {
		meshData.Vertices[i].x = pos[i].x;
		meshData.Vertices[i].y = pos[i].y;
		meshData.Vertices[i].z = pos[i].z;
	}

	for (uint32 i = 0; i < numSubdivisions; ++i)
		Subdivide(meshData);

	// Project vertices onto sphere and scale.
	for (uint32 i = 0; i < meshData.Vertices.size(); ++i)
	{
		// Project onto unit sphere.
		AWEVector n(meshData.Vertices[i].x, meshData.Vertices[i].y, meshData.Vertices[i].z);
		n.Normalize();

		// Project onto sphere.
		AWEVector p = n * radius;
		
		meshData.Vertices[i].x = p.x; meshData.Vertices[i].y = p.y, meshData.Vertices[i].z = p.z;
		meshData.Vertices[i].vcN[0] = n.x; meshData.Vertices[i].vcN[1] = n.y; meshData.Vertices[i].vcN[2] = n.z;

		// Derive texture coordinates from spherical coordinates.
		float theta = atan2f(meshData.Vertices[i].z, meshData.Vertices[i].x);

		// Put in [0, 2pi].
		if (theta < 0.0f)
			theta += XM_2PI;

		float phi = acosf(meshData.Vertices[i].y / radius);

		meshData.Vertices[i].tu = theta / XM_2PI;
		meshData.Vertices[i].tv = phi / XM_PI;
	}

	return meshData;
}

AwesomeGeometryGenerator::AwesomeMeshData AwesomeGeometryGenerator::CreateCylinder(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount)
{
	AwesomeMeshData meshData;

	//
	// Build Stacks.
	// 

	float stackHeight = height / stackCount;

	// Amount to increment radius as we move up each stack level from bottom to top.
	float radiusStep = (topRadius - bottomRadius) / stackCount;

	uint32 ringCount = stackCount + 1;

	// Compute vertices for each stack ring starting at the bottom and moving up.
	for (uint32 i = 0; i < ringCount; ++i)
	{
		float y = -0.5f * height + i * stackHeight;
		float r = bottomRadius + i * radiusStep;

		// vertices of ring
		float dTheta = 2.0f * XM_PI / sliceCount;
		for (uint32 j = 0; j <= sliceCount; ++j)
		{
			VERTEX vertex;

			float c = cosf(j * dTheta);
			float s = sinf(j * dTheta);

			vertex.x = r * c;
			vertex.y = y;
			vertex.z = r * s;

			vertex.tu = (float)j / sliceCount;
			vertex.tv = 1.0f - (float)i / stackCount;

			// Cylinder can be parameterized as follows, where we introduce v
			// parameter that goes in the same direction as the v tex-coord
			// so that the bitangent goes in the same direction as the v tex-coord.
			//   Let r0 be the bottom radius and let r1 be the top radius.
			//   y(v) = h - hv for v in [0,1].
			//   r(v) = r1 + (r0-r1)v
			//
			//   x(t, v) = r(v)*cos(t)
			//   y(t, v) = h - hv
			//   z(t, v) = r(v)*sin(t)
			// 
			//  dx/dt = -r(v)*sin(t)
			//  dy/dt = 0
			//  dz/dt = +r(v)*cos(t)
			//
			//  dx/dv = (r0-r1)*cos(t)
			//  dy/dv = -h
			//  dz/dv = (r0-r1)*sin(t)

			// This is unit length.
			AWEVector TangentU(-s, 0.0f, c);

			float dr = bottomRadius - topRadius;
			AWEVector bitangent(dr * c, -height, dr * s);

			/* this is same as :
			XMVECTOR T = XMLoadFloat3(&vertex.TangentU);
			XMVECTOR B = XMLoadFloat3(&bitangent);
			XMVECTOR N = XMVector3Normalize(XMVector3Cross(T, B));*/
			AWEVector Result;
			Result.x = (TangentU.y * bitangent.z) - (TangentU.z * bitangent.y);
			Result.y = (TangentU.z * bitangent.x) - (TangentU.x * bitangent.z);
			Result.z = (TangentU.x * bitangent.y) - (TangentU.y * bitangent.x);
			Result.w = 0;
			Result.Normalize();

			vertex.vcN[0] = Result.x;
			vertex.vcN[1] = Result.y;
			vertex.vcN[2] = Result.z;

			meshData.Vertices.push_back(vertex);
		}
	}

	// Add one because we duplicate the first and last vertex per ring
	// since the texture coordinates are different.
	uint32 ringVertexCount = sliceCount + 1;

	// Compute indices for each stack.
	for (uint32 i = 0; i < stackCount; ++i)
	{
		for (uint32 j = 0; j < sliceCount; ++j)
		{
			meshData.Indices32.push_back(i * ringVertexCount + j);
			meshData.Indices32.push_back((i + 1) * ringVertexCount + j);
			meshData.Indices32.push_back((i + 1) * ringVertexCount + j + 1);

			meshData.Indices32.push_back(i * ringVertexCount + j);
			meshData.Indices32.push_back((i + 1) * ringVertexCount + j + 1);
			meshData.Indices32.push_back(i * ringVertexCount + j + 1);
		}
	}

	BuildCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);
	BuildCylinderBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);

	return meshData;
}

void AwesomeGeometryGenerator::BuildCylinderTopCap(float bottomRadius, float topRadius, float height,
	uint32 sliceCount, uint32 stackCount, AwesomeMeshData& meshData)
{
	uint32 baseIndex = (uint32)meshData.Vertices.size();

	float y = 0.5f * height;
	float dTheta = 2.0f * XM_PI / sliceCount;

	// Duplicate cap ring vertices because the texture coordinates and normals differ.
	for (uint32 i = 0; i <= sliceCount; ++i)
	{
		float x = topRadius * cosf(i * dTheta);
		float z = topRadius * sinf(i * dTheta);

		// Scale down by the height to try and make top cap texture coord area
		// proportional to base.
		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		meshData.Vertices.push_back(VERTEX(x, y, z, 0.0f, 1.0f, 0.0f, u, v));
	}

	// Cap center vertex.
	meshData.Vertices.push_back(VERTEX(0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f));

	// Index of center vertex.
	uint32 centerIndex = (uint32)meshData.Vertices.size() - 1;

	for (uint32 i = 0; i < sliceCount; ++i)
	{
		meshData.Indices32.push_back(centerIndex);
		meshData.Indices32.push_back(baseIndex + i + 1);
		meshData.Indices32.push_back(baseIndex + i);
	}
}

void AwesomeGeometryGenerator::BuildCylinderBottomCap(float bottomRadius, float topRadius, float height,
	uint32 sliceCount, uint32 stackCount, AwesomeMeshData& meshData)
{
	// 
	// Build bottom cap.
	//

	uint32 baseIndex = (uint32)meshData.Vertices.size();
	float y = -0.5f * height;

	// vertices of ring
	float dTheta = 2.0f * XM_PI / sliceCount;
	for (uint32 i = 0; i <= sliceCount; ++i)
	{
		float x = bottomRadius * cosf(i * dTheta);
		float z = bottomRadius * sinf(i * dTheta);

		// Scale down by the height to try and make top cap texture coord area
		// proportional to base.
		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		meshData.Vertices.push_back(VERTEX(x, y, z, 0.0f, -1.0f, 0.0f, u, v));
	}

	// Cap center vertex.
	meshData.Vertices.push_back(VERTEX(0.0f, y, 0.0f, 0.0f, -1.0f, 0.0f, 0.5f, 0.5f));

	// Cache the index of center vertex.
	uint32 centerIndex = (uint32)meshData.Vertices.size() - 1;

	for (uint32 i = 0; i < sliceCount; ++i)
	{
		meshData.Indices32.push_back(centerIndex);
		meshData.Indices32.push_back(baseIndex + i);
		meshData.Indices32.push_back(baseIndex + i + 1);
	}
}

AwesomeGeometryGenerator::AwesomeMeshData AwesomeGeometryGenerator::CreateGrid(float width, float depth, uint64 m, uint64 n)
{
	AwesomeMeshData meshData;

	uint64 vertexCount = m * n;
	uint64 faceCount = (m - 1) * (n - 1) * 2;

	//
	// Create the vertices.
	//

	float halfWidth = 0.5f * width;
	float halfDepth = 0.5f * depth;

	float dx = width / (n - 1);
	float dz = depth / (m - 1);

	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);

	meshData.Vertices.resize(vertexCount);
	for (uint64 i = 0; i < m; ++i)
	{
		float z = halfDepth - i * dz;
		for (uint64 j = 0; j < n; ++j)
		{
			float x = -halfWidth + j * dx;

			meshData.Vertices[i * n + j].x = x;
			meshData.Vertices[i * n + j].y = 0.0f;
			meshData.Vertices[i * n + j].z = z;

			meshData.Vertices[i * n + j].vcN[0] = 0.0f;
			meshData.Vertices[i * n + j].vcN[1] = 1.0f;
			meshData.Vertices[i * n + j].vcN[2] = 0.0f;

			// Stretch texture over grid.
			meshData.Vertices[i * n + j].tu = j * du;
			meshData.Vertices[i * n + j].tv = i * dv;
		}
	}

	//
	// Create the indices.
	//

	meshData.Indices32.resize(faceCount * 3); // 3 indices per face

	// Iterate over each quad and compute indices.
	uint64 k = 0;
	for (uint64 i = 0; i < m - 1; ++i)
	{
		for (uint64 j = 0; j < n - 1; ++j)
		{
			meshData.Indices32[k] = i * n + j;
			meshData.Indices32[k + 1] = i * n + j + 1;
			meshData.Indices32[k + 2] = (i + 1) * n + j;

			meshData.Indices32[k + 3] = (i + 1) * n + j;
			meshData.Indices32[k + 4] = i * n + j + 1;
			meshData.Indices32[k + 5] = (i + 1) * n + j + 1;

			k += 6; // next quad
		}
	}

	return meshData;
}

AwesomeGeometryGenerator::AwesomeMeshData AwesomeGeometryGenerator::CreateQuad(float x, float y, float w, float h, float depth)
{
	AwesomeMeshData meshData;

	meshData.Vertices.resize(4);
	meshData.Indices32.resize(6);

	// Position coordinates specified in NDC space.
	meshData.Vertices[0] = VERTEX(
		x, y - h, depth,
		0.0f, 0.0f, -1.0f,
		0.0f, 1.0f);

	meshData.Vertices[1] = VERTEX(
		x, y, depth,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f);

	meshData.Vertices[2] = VERTEX(
		x + w, y, depth,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f);

	meshData.Vertices[3] = VERTEX(
		x + w, y - h, depth,
		0.0f, 0.0f, -1.0f,
		1.0f, 1.0f);

	meshData.Indices32[0] = 0;
	meshData.Indices32[1] = 1;
	meshData.Indices32[2] = 2;

	meshData.Indices32[3] = 0;
	meshData.Indices32[4] = 2;
	meshData.Indices32[5] = 3;

	return meshData;
}
