//-----------------------------------------------------------------------
// AwesomeLinearInterpolation.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 23/04/2023
//-----------------------------------------------------------------------
// Desc: Class for linear interpolation on vectors (maybe I should think
// on a better name)

#ifndef AWELINEARANIM_H
#define AWELINEARANIM_H
#pragma once

#include "AwesomeGeneral.h"
class AwesomeLinearInterpolation : public AwesomeBaseAnimation
{
public:
	AwesomeLinearInterpolation();
	virtual ~AwesomeLinearInterpolation();

	AWEVector Update(float fElapsedTime);

	void Set(AWEVector vcStart, AWEVector vcEnd);

private:
	AWEVector m_vcStart;
	AWEVector m_vcEnd;

};

#endif // !AWELINEARANIM_H


