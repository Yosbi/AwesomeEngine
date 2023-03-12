//-----------------------------------------------------------------------
// AwesomeGameTimer.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 12/03/2023
//-----------------------------------------------------------------------
// Desc: Game timer util class and functions, the credits of this class 
// belongs to Frank Luna


#pragma once
class AwesomeGameTimer
{
public:
	AwesomeGameTimer();

	float TotalTime()const; // in seconds
	float DeltaTime()const; // in seconds

	void Reset(); // Call before message loop.
	void Start(); // Call when unpaused.
	void Stop();  // Call when paused.
	void Tick();  // Call every frame.

private:
	double mSecondsPerCount;
	double mDeltaTime;

	__int64 mBaseTime;
	__int64 mPausedTime;
	__int64 mStopTime;
	__int64 mPrevTime;
	__int64 mCurrTime;

	bool mStopped;
};

