#ifndef AWEMESH_H
#define AWEMESH_H
#pragma once
#include <wrl/client.h>
#include <d3d12.h>
#include <directxmath.h>
#include "../AwesomeRenderer/AwesomeRenderDevice.h"

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>


struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT2 texture;
	DirectX::XMFLOAT3 normal;
};

class AweMesh
{
public:
	AweMesh();
	~AweMesh();

	void loadOBJ(std::wstring sFileName);

	void setDefaultVerticesColor(float red, float green, float blue, float alpha);

	std::vector<Vertex> getVerteces();
	std::vector<WORD> getIndex();

	DirectX::XMMATRIX getModelMatrix();

	Microsoft::WRL::ComPtr<ID3D12Resource> getVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D12Resource> getIndexBuffer();

	void setVertexBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> buffer);
	void setIndexBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> buffer);

	D3D12_VERTEX_BUFFER_VIEW* getVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW*  getIndexBufferView();

private:

	std::vector<std::string> GetTokens(std::string sLine, char c);

	std::vector<WORD> m_index;
	std::vector<Vertex> m_verteces;

	DirectX::XMFLOAT4 m_defaultColor;

	DirectX::XMMATRIX m_modelMatrix;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_IndexBuffer;

	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
};
#endif
