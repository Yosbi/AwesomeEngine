#pragma once
#include <d3d12.h>
#include <directxmath.h>

struct OBJVertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texture;
	DirectX::XMFLOAT3 normal;
};