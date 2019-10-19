#ifndef TIMERCLASS_H
#define TIMERCLASS_H

#pragma comment(lib, "winmm.lib")

// INCLUDES //
#include <windows.h>
#include <mmsystem.h>

class TimerClass
{
public:
	TimerClass();
	TimerClass(const TimerClass&);
	~TimerClass();

	bool Initialize(HWND hwnd);
	void Update();

	unsigned int GetElapsedTime();
	unsigned int GetFPS();
	float GetFrameTime();

private:
	// VARIABLES //	
	unsigned int m_FPS, m_ElapsedTime, m_FramesCounter;
	unsigned long m_prevTime;

	INT64 m_frequency;
	float m_ticksPerMs;
	INT64 m_startTime;
	float m_frameTime;
};
	

#endif


