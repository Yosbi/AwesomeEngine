#include "AweMesh.h"

AweMesh::AweMesh() 
{
	m_defaultColor = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_modelMatrix = DirectX::XMMatrixIdentity();

	m_indexBufferView.BufferLocation = 0;
	m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	m_indexBufferView.SizeInBytes = 0;

	m_vertexBufferView.BufferLocation = 0;
	m_vertexBufferView.SizeInBytes = 0;
	m_vertexBufferView.StrideInBytes = 0;
}

AweMesh::~AweMesh() {}

void AweMesh::loadOBJ(std::wstring sFileName)
{
	std::ifstream inFile(sFileName);

	std::vector<DirectX::XMFLOAT3> vertices;
	std::vector<DirectX::XMFLOAT2> textures;
	std::vector<DirectX::XMFLOAT3> normals;

	if (inFile.is_open())
	{
		std::string line;
		while (std::getline(inFile, line))
		{
			std::vector<std::string> tokens = GetTokens(line, ' ');
			if (!tokens.empty())
			{
				// Loading vertices
				if (tokens[0] == "v")
				{
					vertices.push_back(DirectX::XMFLOAT3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])));

					//m_verteces.push_back(Vertex{ DirectX::XMFLOAT3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]))
					//		, m_defaultColor});
				}
				else if (tokens[0] == "vt")
				{
					textures.push_back(DirectX::XMFLOAT2(std::stof(tokens[1]), std::stof(tokens[2])));
				}
				else if (tokens[0] == "vn")
				{
					normals.push_back(DirectX::XMFLOAT3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])));
				}
				else if (tokens[0] == "f")
				{
					for (int i = 1; i < 4; i++)
					{
						std::vector<std::string> faceV = GetTokens(tokens[i], '/');

						Vertex vertex;
						vertex.position = vertices[std::stoi(faceV[0]) - 1];
						vertex.color = m_defaultColor;
						vertex.texture = textures[std::stoi(faceV[1]) - 1];
						vertex.normal = normals[std::stoi(faceV[2]) - 1];

						m_verteces.push_back(vertex);
						m_index.push_back(std::stoi(faceV[0]) - 1);
					}
				}
			}
		}
		//std::reverse(m_verteces.begin(), m_verteces.end());
		//std::reverse(m_index.begin(), m_index.end());
	}
	else 
	{
		/* TODO: handle error */
	}
	
	
}

void AweMesh::setDefaultVerticesColor(float red, float green, float blue, float alpha)
{
	m_defaultColor = DirectX::XMFLOAT4(red, green, blue, alpha);
}

std::vector<Vertex> AweMesh::getVerteces()
{
	return m_verteces;
}

std::vector<WORD> AweMesh::getIndex()
{
	return m_index;
}

DirectX::XMMATRIX AweMesh::getModelMatrix()
{
	return m_modelMatrix;
}

std::vector<std::string> AweMesh::GetTokens(std::string sLine, char c)
{
	// Vector of string to save tokens
	std::vector <std::string> tokens;

	// stringstream class check1
	std::stringstream check1(sLine.c_str());

	std::string intermediate;

	// Tokenizing
	while (getline(check1, intermediate, c))
	{
		tokens.push_back(intermediate);
	}

	return tokens;
}

Microsoft::WRL::ComPtr<ID3D12Resource> AweMesh::getVertexBuffer()
{
	return m_vertexBuffer;
}

Microsoft::WRL::ComPtr<ID3D12Resource> AweMesh::getIndexBuffer()
{
	return m_IndexBuffer;
}

void AweMesh::setVertexBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> buffer)
{
	m_vertexBuffer = buffer;
}

void AweMesh::setIndexBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> buffer)
{
	m_IndexBuffer = buffer;
}

D3D12_VERTEX_BUFFER_VIEW* AweMesh::getVertexBufferView()
{
	return &m_vertexBufferView;
}

D3D12_INDEX_BUFFER_VIEW* AweMesh::getIndexBufferView()
{
	return &m_indexBufferView;
}
