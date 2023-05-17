
#include "stdafx.h"
#include "GameFramework.h"
#include "MyUtility.h"
#include "Scene.h"
#include "Camera.h"
#include "Transform.h"
#include "Component.h"
#include "GraphicsPipeline.h"

#include "Timer.h"
#include "Key.h"

#include "GameManager.h"



// ½Ì±ÛÅæ static º¯¼ö Á¤ÀÇ 
CGameFramework* CGameFramework::m_pInst = nullptr;


CGameFramework::CGameFramework()
{
}

CGameFramework::~CGameFramework()
{
	if (m_pCurScene != nullptr)
	{
		delete m_pCurScene;
		m_pCurScene = nullptr;
	}



}

void CGameFramework::Init(HINSTANCE hInstance, HWND hMainWnd)
{
	OnCreate(hInstance, hMainWnd);
	CGameTimer::GetInst()->Init();
	Key::GetInst()->init();

}

void CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	std::srand(timeGetTime());

	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	BuildFrameBuffer();

	if (m_pCurScene == nullptr) {
		m_pCurScene = new CScene;
		m_pCurScene->Init();

	}
	_tcscpy_s(m_pszFrameRate, _T("LabProject ("));

}

void CGameFramework::Update(const float& _fTimeElapsed)
{
	if (m_pCurScene == nullptr)
		return;

	m_pCurScene->Update(_fTimeElapsed);



}

void CGameFramework::FinalUpdate(float _fTimeElapsed)
{
	m_pCurScene->FinalUpdate(_fTimeElapsed);

}

void CGameFramework::Render(HDC FrameBuffer, const float& _fTimeElapsed)
{
	CGameObject*	pCameraObj				= m_pCurScene->GetMainCamera();
	CCamera*		pCam					= (CCamera*)pCameraObj->GetComponent(COMPONENT_TYPE::CAMERA);	

	CViewport viewport						= pCam->GetViewport();
	XMFLOAT4X4 ViewPerspecitveProjectMat	= pCam->GetViewPerspectiveProjectMat();

	XMFLOAT3 dir = XMFLOAT3(1.f, 1.f, 1.f);

	CGraphicsPipeline::SetViewport(&(viewport));
	CGraphicsPipeline::SetViewPerspectiveProjectTransform(&ViewPerspecitveProjectMat);

	m_pCurScene->Render(FrameBuffer, _fTimeElapsed);

}



void CGameFramework::OnDestroy()
{


}

void CGameFramework::BuildFrameBuffer()
{
	::GetClientRect(m_hWnd, &m_rcClient);

	HDC hDC = ::GetDC(m_hWnd);

	m_hDCFrameBuffer = ::CreateCompatibleDC(hDC);
	m_hBitmapFrameBuffer = ::CreateCompatibleBitmap(hDC, m_rcClient.right - m_rcClient.left, m_rcClient.bottom - m_rcClient.top);
	::SelectObject(m_hDCFrameBuffer, m_hBitmapFrameBuffer);

	::ReleaseDC(m_hWnd, hDC);
	::SetBkMode(m_hDCFrameBuffer, TRANSPARENT);

}

void CGameFramework::ClearFrameBuffer(DWORD dwColor)
{
	HPEN hPen = ::CreatePen(PS_SOLID, 0, dwColor);
	HPEN hOldPen = (HPEN)::SelectObject(m_hDCFrameBuffer, hPen);
	HBRUSH hBrush = ::CreateSolidBrush(dwColor);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(m_hDCFrameBuffer, hBrush);
	::Rectangle(m_hDCFrameBuffer, m_rcClient.left, m_rcClient.top, m_rcClient.right, m_rcClient.bottom);
	::SelectObject(m_hDCFrameBuffer, hOldBrush);
	::SelectObject(m_hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);
	::DeleteObject(hBrush);

}

void CGameFramework::PresentFrameBuffer()
{
	HDC hDC = ::GetDC(m_hWnd);
	::BitBlt(hDC, m_rcClient.left, m_rcClient.top, m_rcClient.right - m_rcClient.left, m_rcClient.bottom - m_rcClient.top, m_hDCFrameBuffer, m_rcClient.left, m_rcClient.top, SRCCOPY);
	::ReleaseDC(m_hWnd, hDC);

}



void CGameFramework::BuildObjects()
{
}

void CGameFramework::ReleaseObjects()
{
}

void CGameFramework::ProcessInput()
{
}

void CGameFramework::AnimateObjects()
{
}

void CGameFramework::StartFrame()
{
	CGameTimer::GetInst()->Tick(60.f);
}

void CGameFramework::EndFrame()
{
	CGameTimer::GetInst()->GetFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);

}

void CGameFramework::FrameAdvance()
{
// START
	StartFrame();
	float fTimeElapsed = CGameTimer::GetInst()->GetTimeElapsed();

// UPDATE
	Key::GetInst()->update();
	Update(fTimeElapsed);
	FinalUpdate(fTimeElapsed);

// RENDER 
	ClearFrameBuffer((DWORD)COLOR::LIGHT_GREY);
	Render(m_hDCFrameBuffer, fTimeElapsed);
	PresentFrameBuffer();

// END
	EndFrame();

	GameManager::GetInst()->CheckEndScene();

}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	//if (m_pCurScene) m_pCurScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);

	switch (nMessageID)
	{
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		if (nMessageID == WM_RBUTTONDOWN)
		{
			//m_pLockedObject = m_pCurScene->PickObjectPointedByCursor(LOWORD(lParam), HIWORD(lParam), m_pPlayer->m_pCamera);
			//if (m_pLockedObject) m_pLockedObject->SetColor(RGB(0, 0, 0));
		}
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	//if (m_pCurScene) m_pCurScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);

	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_RETURN:
			break;
		case 'R':
			//((CTankPlayer*)m_pPlayer)->RotateTurret(+5.0f);
			break;
		case 'E':
			//((CTankPlayer*)m_pPlayer)->RotateTurret(-5.0f);
			break;
		case 'Q':
			//((CTankPlayer*)m_pPlayer)->RotateGun(+1.0f);
			break;
		case 'W':
			//((CTankPlayer*)m_pPlayer)->RotateGun(-1.0f);
			break;
		case VK_CONTROL:
			//((CTankPlayer*)m_pPlayer)->FireBullet(m_pLockedObject);
			//m_pLockedObject = NULL;
			break;
		default:
			//m_pCurScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
			break;
		}
		break;
	default:
		break;
	}
}

LRESULT CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_ACTIVATE:
	{
		if (LOWORD(wParam) == WA_INACTIVE)
			CGameTimer::GetInst()->Stop();
		else
			CGameTimer::GetInst()->Start();
		break;
	}
	case WM_SIZE:
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}
