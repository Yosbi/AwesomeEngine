//-----------------------------------------------------------------------
// AwesomeBezier.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 29/05/2023
//-----------------------------------------------------------------------
// Desc: Class for Bezier curves for animations

#ifndef AWEBEZIERANIM_H
#define AWEBEZIERANIM_H
#pragma once

#include "AwesomeGeneral.h"
#include <vector>
class AwesomeBezier : public AwesomeBaseAnimation
{
public:
	AwesomeBezier();
	virtual ~AwesomeBezier();

	AWEVector Update(float fElapsedTime);

	void Set(std::vector<AWEVector> vcPoint, float totalTimeAnim = 1);

private:
	std::vector <AWEVector> m_vcPoints;
	float m_fTotalTimeAnim;


};

#endif // !AWEBEZIERANIM_H


