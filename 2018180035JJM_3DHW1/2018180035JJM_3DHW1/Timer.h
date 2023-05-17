#pragma once
//-----------------------------------------------------------------------------
// File: CGameTimer.h
//-----------------------------------------------------------------------------

const ULONG MAX_SAMPLE_COUNT = 50; // Maximum frame time sample count

class CGameTimer
{
public:
	CGameTimer();
	virtual ~CGameTimer();


private:
	// SingleTon
	static CGameTimer* m_pInst;
public:
	static CGameTimer* GetInst() {
		if (nullptr == m_pInst) 
			m_pInst = new CGameTimer;
		return m_pInst;
	}
	static void Destroy() {
		if (nullptr != m_pInst) {
			delete m_pInst;
			m_pInst = nullptr;
		}
	}

public:
	void Init();
	void Update(float fLockFPS = 0.f);



	void Tick(float fLockFPS = 0.0f);
	void Start();
	void Stop();
	void Reset();

	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0);
	float GetTimeElapsed();
	float GetTotalTime();

private:
	double							m_fTimeScale;
	float							m_fTimeElapsed;

	__int64							m_nBasePerformanceCounter;
	__int64							m_nPausedPerformanceCounter;
	__int64							m_nStopPerformanceCounter;
	__int64							m_nCurrentPerformanceCounter;
	__int64							m_nLastPerformanceCounter;

	__int64							m_PerformanceFrequencyPerSec;

	float							m_fFrameTime[MAX_SAMPLE_COUNT]{0};
	ULONG							m_nSampleCount;

	unsigned long					m_nCurrentFrameRate;
	unsigned long					m_FramePerSecond;
	float							m_fFPSTimeElapsed;

	bool							m_bStopped;
};
