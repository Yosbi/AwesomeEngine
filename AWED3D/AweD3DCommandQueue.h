//-----------------------------------------------------------------------
// AweD3DCommandQueue.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 12/03/2023
//-----------------------------------------------------------------------
// Desc: This is a wrapper class for the ID3D12CommandQueue

#ifndef AWED3DCOMMANDQUEUE_H
#define AWED3DCOMMANDQUEUE_H

#include <d3d12.h>
#include <wrl.h>

#include <cstdint>
#include <queue>

#include "AweD3DException.h"

#pragma once
class AweD3DCommandQueue
{
public:
	AweD3DCommandQueue(Microsoft::WRL::ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);
	virtual ~AweD3DCommandQueue();

    // Get an available command list from the command queue.
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> GetCommandList();

    // Execute a command list.
    // Returns the fence value to wait for for this command list.
    uint64_t ExecuteCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList);

    uint64_t Signal();
    bool IsFenceComplete(uint64_t fenceValue);
    void WaitForFenceValue(uint64_t fenceValue);
    void Flush();

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetD3D12CommandQueue() const;

private:
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator>      CreateCommandAllocator();
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2>  CreateCommandList(Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator);

    struct CommandAllocatorEntry
    {
        uint64_t fenceValue;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
    };


    D3D12_COMMAND_LIST_TYPE                     m_CommandListType;
    Microsoft::WRL::ComPtr<ID3D12Device2>       m_d3d12Device;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue>  m_d3d12CommandQueue;
    Microsoft::WRL::ComPtr<ID3D12Fence>         m_d3d12Fence;
    HANDLE                                      m_FenceEvent;
    uint64_t                                    m_nFenceValue;

    // Queues
    std::queue<CommandAllocatorEntry>                                   m_CommandAllocatorQueue;
    std::queue< Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> >    m_CommandListQueue;
};

#endif // !AWED3DCOMMANDQUEUE_H



