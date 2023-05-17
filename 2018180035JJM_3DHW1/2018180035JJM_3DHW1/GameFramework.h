#pragma once

#include "Timer.h"

class CScene;

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

private:
	// SingleTon
	static CGameFramework* m_pInst;
public:
	static CGameFramework* GetInst() {
		if (nullptr == m_pInst) m_pInst = new CGameFramework;
		return m_pInst;
	}
	static void Destroy() {
		if (nullptr != m_pInst) {
			delete m_pInst;
			m_pInst = nullptr;
		}
	}

private:
// ACTIVE
	bool						m_bActive = true;

// WINDOW
	HINSTANCE					m_hInstance = NULL;
	HWND						m_hWnd      = NULL;
	RECT						m_rcClient;

// FRAME BUFFER 
	HDC							m_hDCFrameBuffer     = NULL;
	HBITMAP						m_hBitmapFrameBuffer = NULL;
	HBITMAP						m_hBitmapSelect      = NULL;

// CURSOR 
	POINT						m_ptOldCursorPos;

// FRAME RATE 
	_TCHAR						m_pszFrameRate[50];

private:
// SCENE
	CScene*						m_pCurScene; // ÇöÀç Scene


public:
	HWND GetMainHwnd() { return m_hWnd; }
	
	void Init(HINSTANCE hInstance, HWND hMainWnd);
	void Update(const float& _fTimeElapsed);
	void FinalUpdate(float _fTimeElapsed);
	void Render(HDC FrameBuffer, const float& _fTimeElapsed);

	void StartFrame();
	void EndFrame();


	void OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	CScene* GetCurScene() { return m_pCurScene; }


public:

	void BuildFrameBuffer();
	void ClearFrameBuffer(DWORD dwColor);
	void PresentFrameBuffer();


	void BuildObjects();
	void ReleaseObjects();


	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();


	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);


	void SetActive(bool bActive) { m_bActive = bActive; }

};

