#include "stdafx.h"
#include "GameManager.h"

// �̱��� static ���� ���� 
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
		int check = MessageBox(nullptr, L"���� ����",
			L"���� ����", MB_ICONQUESTION | MB_OKCANCEL);
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
