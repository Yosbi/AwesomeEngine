//-----------------------------------------------------------------------
// AweD3DShader.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 22/05/2023
//-----------------------------------------------------------------------
//Desc: In this file there are definitions to load shaders and create
// pipeline state objects
//--------------------------------------------------------------------------
#include "AweD3D.h"

ID3D12PipelineState* AweD3D::GetActivePipelineStateObject() {
	return m_vcPipelineState.at(m_nPipelineStateIndex).Get();

}

HRESULT AweD3D::SetActivePipelineStateObject(UINT nID) {
	if (nID >= m_vcPipelineState.size())
		return E_FAIL;

	m_nPipelineStateIndex = nID;

	return S_OK;
}

HRESULT AweD3D::CreatePipelineStateObject(std::wstring sVertexShader, std::wstring sPixelShader, bool bWireframe, UINT &nID)
{
#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob;

	// Load the vertex shader.
	ThrowIfFailed(D3DReadFileToBlob(GetAssetFullPath(sVertexShader.c_str()).c_str(), &vertexShaderBlob));

	// Load the pixel shader.
	ThrowIfFailed(D3DReadFileToBlob(GetAssetFullPath(sPixelShader.c_str()).c_str(), &pixelShaderBlob));

	if (FAILED(CreatePipelineStateObject(vertexShaderBlob, pixelShaderBlob, bWireframe)))
		return E_ABORT;

	nID = (UINT)(m_vcPipelineState.size() - 1);

	return S_OK;
}

HRESULT AweD3D::CreatePipelineStateObject(Microsoft::WRL::ComPtr<ID3DBlob> vertexShader, Microsoft::WRL::ComPtr<ID3DBlob> pixelShader, bool bWireframe)
{
	// Define the vertex input layout.
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		//{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

	};

	// Describe and create the graphics pipeline state object (PSO).
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = m_rootSignature.Get();
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	if (bWireframe)
	{
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	}
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.DSVFormat = m_DepthStencilFormat;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pPipelineState;
	try {
		ThrowIfFailed(m_d3d12Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pPipelineState)));
	}
	catch (std::exception& e)
	{
		return E_ABORT;
	}
	

	m_vcPipelineState.emplace_back(pPipelineState);
	return S_OK;

}
