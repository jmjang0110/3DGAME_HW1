#include "stdafx.h"
#include "GameManager.h"

// 싱글톤 static 변수 정의 
GameManager* GameManager::m_pInst = nullptr;



GameManager::GameManager()
{
}


GameManager::~GameManager()
{


}

void GameManager::CheckEndScene()
{
	if (m_iMonsterMaxCnt <= m_iDie_MonsterCnt) {
		int check = MessageBox(nullptr, L"게임 종료",
			L"게임 종료", MB_ICONQUESTION | MB_OKCANCEL);
		if (check) {
			PostQuitMessage(0);
		}
	}

}

void GameManager::Init()
{


}

void GameManager::Update()
{


}
