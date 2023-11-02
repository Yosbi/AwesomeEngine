//-----------------------------------------------------------------------
// AwesomeTimer.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 22/04/2023
//-----------------------------------------------------------------------
// Desc: General timer implementation for games and animations

#ifndef  AWETIMER_H
#define  AWETIMER_H
#pragma once

#include <stdio.h>
#include <string>
#include <windows.h>
#include <mmsystem.h>

class AwesomeTimer {
public:
    AwesomeTimer();
    AwesomeTimer(unsigned char nHH, unsigned char nMM);
    ~AwesomeTimer();

    void  Update(void);

    void  SetClock(unsigned char nHH,
        unsigned char nMM);

    void GetClock(unsigned char& nHH, unsigned char& nMM, unsigned char& mSS);

    float GetTimeStamp(void) { return m_fStamp; }
    float GetElapsed(void) { return (m_fTimeElapsed * m_fFactor); }
    float GetFPS(void) { return (1 / m_fTimeElapsed); }
    float GetScale(void) { return m_fFactor; }
    void  SetScale(float fFactor) { m_fFactor = fFactor; }

private:
    LONGLONG m_nCurTime;          // current time
    LONGLONG m_nPerfCnt;          // performance timer frequency
    LONGLONG m_nLastTime;	      // time of previous frame
    double   m_fTimeElapsed;      // time elapsed since previous frame
    double   m_fTimeScale;        // scaling factor for time
    UCHAR    m_nHH;               // clock time hours
    UCHAR    m_nMM;               // clock time minutes
    UCHAR    m_nSS;               // clock time seconds
    double   m_fClock;            // sum up milliseconds
    double   m_fFactor;           // slowmo or speedup
    double   m_fStamp;            // unique timestamp
};

#endif