#include "AwesomeOBJLoader.h"
#include "AwesomeUtility.h"


AwesomeOBJLoader::AwesomeOBJLoader()
{
}

AwesomeOBJLoader::~AwesomeOBJLoader() {}

int AwesomeOBJLoader::getVertexSize()
{
	return sizeof(VERTEX);
}

void AwesomeOBJLoader::loadOBJ(std::wstring sFileName)
{
	std::ifstream inFile(GetAssetFullPath(sFileName.c_str()));

	std::vector<PVERTEX> vertices;
	std::vector<TEXTURE_COORDS> textures;
	std::vector<PVERTEX> normals;

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
					PVERTEX pvertex;
					pvertex.x = std::stof(tokens[1]); pvertex.y = std::stof(tokens[2]); pvertex.z = std::stof(tokens[3]);
					vertices.push_back(pvertex);

					//m_verteces.push_back(Vertex{ DirectX::XMFLOAT3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]))
					//		, m_defaultColor});
				}
				else if (tokens[0] == "vt")
				{
					TEXTURE_COORDS tcords;
					tcords.tu = std::stof(tokens[1]); tcords.tv = std::stof(tokens[2]);
					textures.push_back(tcords);
				}
				else if (tokens[0] == "vn")
				{
					PVERTEX pnormals;
					pnormals.x = std::stof(tokens[1]); pnormals.y = std::stof(tokens[2]); pnormals.z = std::stof(tokens[3]);
					normals.push_back(pnormals);
				}
				else if (tokens[0] == "f")
				{
					for (int i = 1; i < 4; i++)
					{
						std::vector<std::string> faceV = GetTokens(tokens[i], '/');

						VERTEX vertex;
						vertex.x = vertices[std::stoi(faceV[0]) - 1].x;
						vertex.y = vertices[std::stoi(faceV[0]) - 1].y;
						vertex.z = vertices[std::stoi(faceV[0]) - 1].z;

						//vertex.color = m_defaultColor;
						vertex.tu = textures[std::stoi(faceV[1]) - 1].tu;
						vertex.tv = textures[std::stoi(faceV[1]) - 1].tv;

						vertex.vcN[0] = normals[std::stoi(faceV[2]) - 1].x;
						vertex.vcN[1] = normals[std::stoi(faceV[2]) - 1].y;
						vertex.vcN[2] = normals[std::stoi(faceV[2]) - 1].z;

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

std::vector<VERTEX> AwesomeOBJLoader::getVerteces()
{
	return m_verteces;
}

std::vector<WORD> AwesomeOBJLoader::getIndex()
{
	return m_index;
}

std::vector<std::string> AwesomeOBJLoader::GetTokens(std::string sLine, char c)
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
