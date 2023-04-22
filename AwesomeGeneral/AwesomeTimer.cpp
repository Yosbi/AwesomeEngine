//-----------------------------------------------------------------------
// AwesomeTimer.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 22/04/2023
//-----------------------------------------------------------------------
// Desc: General timer implementation for games and animations
#include "AwesomeGeneral.h"


//--------------------------------------------------------------------
// Name: Constructor
// Desc: Init the timer
//--------------------------------------------------------------------
AwesomeTimer::AwesomeTimer()
{
    AwesomeTimer(0, 0);
}

//--------------------------------------------------------------------
// Name: Constructor
// Desc: Init the timer
//--------------------------------------------------------------------
AwesomeTimer::AwesomeTimer(unsigned char nHH, unsigned char nMM) {
    m_fClock = 0.0f;
    m_nCurTime = 0;
    m_nLastTime = 0;
    m_fTimeElapsed = 1.0f;
    m_fFactor = 1.0f;
    m_fStamp = 0.0f;

    // set clock
    if (nHH > 23)
        nHH = 0;
    if (nMM > 59)
        nMM = 0;

    m_nHH = nHH;
    m_nMM = nMM;
    m_nSS = 0;

    // either QueryPerformanceCounter or timeGetTime 
    QueryPerformanceFrequency((LARGE_INTEGER*)&m_nPerfCnt);
    QueryPerformanceCounter((LARGE_INTEGER*)&m_nLastTime);
    m_fTimeScale = 1.0f / (double)m_nPerfCnt;
} 

//--------------------------------------------------------------------
// Name: Destructor
// Desc: 
//--------------------------------------------------------------------
AwesomeTimer::~AwesomeTimer() {
    /* nothing */
}

//--------------------------------------------------------------------
// Name: Update
// Desc: Counts the actual time and adjusts clock.
//--------------------------------------------------------------------
void AwesomeTimer::Update() {
    QueryPerformanceCounter((LARGE_INTEGER*)&m_nCurTime);
    
    m_fTimeElapsed = (m_nCurTime - m_nLastTime)
        * m_fTimeScale;
    m_nLastTime = m_nCurTime;

    // adjust clock by seconds passed
    m_fClock += (m_fTimeElapsed * m_fFactor);
    if (m_fClock >= 1.0f) {
        m_nSS++;
        m_fClock = 0.0f;
    }
    if (m_nSS >= 60) {
        m_nMM++;
        m_nSS = 0;
    }
    if (m_nMM >= 60) {
        m_nHH++;
        m_nMM = 0;
    }
    if (m_nHH >= 24)
        m_nHH = 0;

    // calculate timestamp from clock and 2 random digits
    m_fStamp = 0.0f;
    m_fStamp += m_fClock;
    m_fStamp += m_nSS;
    m_fStamp += m_nMM * 100;     // move left 2 decimal positions 
    m_fStamp += m_nHH * 10000;   // move left 4 decimal positions
} 

//--------------------------------------------------------------------
// Name: SetClock
// Desc: Sets the clock to any given starting time.
//--------------------------------------------------------------------
void AwesomeTimer::SetClock(unsigned char nHH, unsigned char nMM) {
    // set clock
    if (nHH > 23)
        nHH = 0;
    if (nMM > 59)
        nMM = 0;

    m_nHH = nHH;
    m_nMM = nMM;
} 

//--------------------------------------------------------------------
// Name: GetClock
// Desc: Returns the hours, minutes and seconds
//--------------------------------------------------------------------
void AwesomeTimer::GetClock(unsigned char& nHH, unsigned char& nMM, unsigned char& nSS) 
{
    nHH = m_nHH;
    nMM = m_nMM;
    nSS = m_nSS;
} 