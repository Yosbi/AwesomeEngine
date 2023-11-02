//-----------------------------------------------------------------------
// Waves.cpp
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 14/05/2023
//-----------------------------------------------------------------------
// Based on the Frank Luna's wave code
#ifndef WAVES_H
#define WAVES_H
#pragma once 

#include "..\..\Graphics\AwesomeRenderer\AwesomeRenderer.h"
#include "..\..\Math\AwesomeMath\AwesomeMath.h"
#include <vector>

class Waves
{
public:
    Waves(int m, int n, float dx, float dt, float speed, float damping);
    Waves(const Waves& rhs) = delete;
    Waves& operator=(const Waves& rhs) = delete;
    ~Waves();

    int RowCount()const;
    int ColumnCount()const;
    long VertexCount()const;
    long TriangleCount()const;
    float Width()const;
    float Depth()const;

    // Returns the solution at the ith grid point.
    const AWEVector& Position(int i)const { return mCurrSolution[i]; }

    // Returns the solution normal at the ith grid point.
    const AWEVector& Normal(int i)const { return mNormals[i]; }

    // Returns the unit tangent vector at the ith grid point in the local x-axis direction.
    const AWEVector& TangentX(int i)const { return mTangentX[i]; }

    void Update(float dt);
    void Disturb(int i, int j, float magnitude);

private:
    int mNumRows = 0;
    int mNumCols = 0;

    long mVertexCount = 0;
    long mTriangleCount = 0;

    // Simulation constants we can precompute.
    float mK1 = 0.0f;
    float mK2 = 0.0f;
    float mK3 = 0.0f;

    float mTimeStep = 0.0f;
    float mSpatialStep = 0.0f;

    std::vector<AWEVector> mPrevSolution;
    std::vector<AWEVector> mCurrSolution;
    std::vector<AWEVector> mNormals;
    std::vector<AWEVector> mTangentX;
};

#endif // WAVES_H