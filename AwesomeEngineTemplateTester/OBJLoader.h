#pragma once
#include "OBJVertex.h"
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

class OBJLoader
{
public:
	OBJLoader();
	~OBJLoader();

	std::vector<OBJVertex> LoadOBJ(std::wstring sFileName);

private:

	std::vector<std::string> GetTokens(std::string sLine, char c);
};

