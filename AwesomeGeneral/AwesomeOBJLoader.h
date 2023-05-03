#ifndef AWEMESH_H
#define AWEMESH_H
#pragma once
#include <wrl/client.h>
#include "../AwesomeRenderer/AwesomeRenderDevice.h"

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>


class AwesomeOBJLoader
{
public:
	AwesomeOBJLoader();
	~AwesomeOBJLoader();

	void loadOBJ(std::wstring sFileName);

	//void setDefaultVerticesColor(float red, float green, float blue, float alpha);

	std::vector<VERTEX> getVerteces();
	std::vector<WORD> getIndex();

private:

	std::vector<std::string> GetTokens(std::string sLine, char c);

	std::vector<WORD> m_index;
	std::vector<VERTEX> m_verteces;

	typedef struct TEXURECOORDS_TYPE {
		float tu;
		float tv;
	} TEXTURE_COORDS;


};
#endif
