#pragma once
#include "Script.h"
class CScene;

class GameManager
{

public:
	GameManager();
	~GameManager();

private:
	// SingleTon
	static GameManager* m_pInst;
public:
	static GameManager* GetInst() {
		if (nullptr == m_pInst) m_pInst = new GameManager;
		return m_pInst;
	}
	static void Destroy() {
		if (nullptr != m_pInst) {
			delete m_pInst;
			m_pInst = nullptr;
		}
	}

private:
    CScene* m_pScene;
    int m_iMonsterMaxCnt = 0;

	int m_iDie_MonsterCnt = 0;

public:
	void AddDieMonster() { m_iDie_MonsterCnt++; }
	void AddMonsterMaxCnt(int n) { m_iMonsterMaxCnt += n; }

	void CheckEndScene();

public:

    void Init();
    void Update();
 

};

