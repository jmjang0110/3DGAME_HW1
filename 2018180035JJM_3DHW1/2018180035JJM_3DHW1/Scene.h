#pragma once

#include "global.h"
#include "Camera.h"


class CGameObject;


class CScene
{
private:
	std::vector<CGameObject*>	m_vecObjects;
	CGameObject*				m_pMainCamera;

	std::vector<CGameObject*>   m_vecMonsters;

public:
	CScene();
	~CScene();

public:
	void Init();

	void Update(float _fTimeElapsed);
	void FinalUpdate(float _fTimeElapsed);

	void Render(HDC FrameBuffer, float _fTimeElapsed);

	void AddObject(CGameObject* pObj);
	CGameObject* GetObjByName(std::string name);



public:
	void AddTankObject();
	void AddFloorObject();
	void AddMainCamera();
	void AddAirplaneObject();
	void ExplosiveObject();
	void AddTankMonsterObject();

	void AddBoxObject();
	float RandF(float fMin, float fMax)
	{
		return(fMin + ((float)rand() / (float)RAND_MAX) * (fMax - fMin));
	}

	void MakeMainStage();

	CGameObject* GetMainCamera();
	void SetMainCamera(CGameObject* pObj);



public:

	void CheckObjectByObjectCollisions();
	void CheckObjectByBulletCollisions();
	void CheckObjectByMonsterBulletCollisions();


public:


};

