//-----------------------------------------------------------------------
// AweD3DUploadBuffer.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 04/05/2023
//-----------------------------------------------------------------------
// Desc: Upload buffer for constant buffer views CBV, this code was inspired
// by Frank Luna

#ifndef AWED3DUPLOADBUFFER_H
#define AWED3DUPLOADBUFFER_H

#pragma once

#include "d3dx12.h"
#include "AweD3DException.h"


template<typename T>
class AweD3DUploadBuffer
{
public:
    AweD3DUploadBuffer(ID3D12Device* device, UINT elementCount, bool isConstantBuffer);

    AweD3DUploadBuffer(const AweD3DUploadBuffer& rhs) = delete;
    AweD3DUploadBuffer& operator=(const AweD3DUploadBuffer& rhs) = delete;
    ~AweD3DUploadBuffer();

    ID3D12Resource* Resource()const;

    void CopyData(int elementIndex, const T& data);

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> mUploadBuffer;
    BYTE* mMappedData = nullptr;

    UINT mElementByteSize = 0;
    bool mIsConstantBuffer = false;
};

#endif // !AWED3DUPLOADBUFFER_H
