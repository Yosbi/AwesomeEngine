//-----------------------------------------------------------------------
// AwesomeBaseAnimation.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 23/04/2023
//-----------------------------------------------------------------------
// Desc: this class is the base for basic animations

#pragma once
#ifndef AWEBASEANIM_H
#define AWEBASEANIM_H
#pragma once

#include "../AwesomeMath/AwesomeMath.h"

class AwesomeBaseAnimation {
public:
    AwesomeBaseAnimation();
    virtual ~AwesomeBaseAnimation();

    bool GetAnimationFinished(void) { return m_bAnimationFinished; }

protected:
    float m_fTotalTime;
    bool m_bAnimationFinished;
};

#endif
