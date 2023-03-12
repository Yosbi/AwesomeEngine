//-----------------------------------------------------------------------
// AweD3DException.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 12/03/2023
//-----------------------------------------------------------------------
// Desc: This is a wrapper class for the ThrowIfFailed method standard in
// DirectX 12

#ifndef AWEDXEXEPTION_H
#define AWEDXEXEPTION_H
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <comdef.h>

#include <string>

class AweDxException
{
public:
    AweDxException() = default;
    AweDxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber);

    std::wstring ToString()const;

    HRESULT ErrorCode = S_OK;
    std::wstring FunctionName;
    std::wstring Filename;
    int LineNumber = -1;
};


inline std::wstring AnsiToWString(const std::string& str)
{
    WCHAR buffer[1024];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 1024);
    return std::wstring(buffer);
}


#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw AweDxException(hr__, L#x, wfn, __LINE__); } \
}
#endif


#endif // !AWEDXEXEPTION_H