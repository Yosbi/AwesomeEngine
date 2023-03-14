#include "OBJLoader.h"

OBJLoader::OBJLoader() {}


OBJLoader::~OBJLoader() {}

std::vector<OBJVertex> OBJLoader::LoadOBJ(std::wstring sFileName)
{
	std::ifstream inFile(sFileName);

	std::vector<DirectX::XMFLOAT3> vertices;
	std::vector<DirectX::XMFLOAT2> textures;
	std::vector<DirectX::XMFLOAT3> normals;

	std::vector<OBJVertex> objVerteces;


	if(inFile.is_open())
	{
		std::string line;
		while (inFile >> line)
		{
			std::vector<std::string> tokens = GetTokens(line, ' ');
			if (!tokens.empty())
			{
				// Loading vertices
				if (tokens[0] == "v")
				{
					vertices.push_back(DirectX::XMFLOAT3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])));
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
					for (int i = 1; i < 5; i++)
					{
						std::vector<std::string> faceV = GetTokens(tokens[i], '/');

						OBJVertex objVertex;
						objVertex.position = vertices[std::stoi(faceV[0]) - 1];
						objVertex.texture = textures[std::stoi(faceV[1]) - 1];
						objVertex.normal = normals[std::stoi(faceV[2]) - 1];
						
						objVerteces.push_back(objVertex);
					}
				}
			}
		}
	}


	return objVerteces;
}


std::vector<std::string> OBJLoader::GetTokens(std::string sLine, char c)
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

