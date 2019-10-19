#include "TimerClass.h"


TimerClass::TimerClass()
{
}

TimerClass::TimerClass(const TimerClass& other)
{}

TimerClass::~TimerClass()
{
}

bool TimerClass::Initialize(HWND hwnd)
{
	// time
	m_prevTime = timeGetTime();
	m_ElapsedTime = 0;

	// fps
	m_FPS =0;
	m_FramesCounter = 0;

	// Check to see if this system supports high performance timers.
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
	if(m_frequency == 0)
	{
		return false;
	}

	// Find out how many times the frequency counter ticks every millisecond.
	m_ticksPerMs = (float)(m_frequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);


	return true;
}

void TimerClass::Update()
{
	m_FramesCounter++;

	if(timeGetTime() >= m_prevTime + 1000)
	{
		// time
		m_prevTime = timeGetTime();
		m_ElapsedTime++;

		// fps
		m_FPS = m_FramesCounter;
		m_FramesCounter = 0;
	}

	INT64 currentTime;
	float timeDifference;


	QueryPerformanceCounter((LARGE_INTEGER*)& currentTime);

	timeDifference = (float)(currentTime - m_startTime);

	m_frameTime = timeDifference / m_ticksPerMs;

	m_startTime = currentTime;


	return;
}

unsigned int TimerClass::GetElapsedTime()
{
	return m_ElapsedTime;
}

unsigned int TimerClass::GetFPS()
{
	return m_FPS;
}

float TimerClass::GetFrameTime()
{
	return m_frameTime;
}