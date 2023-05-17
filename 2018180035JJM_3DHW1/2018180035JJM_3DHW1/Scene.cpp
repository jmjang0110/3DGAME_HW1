#include "Scene.h"
#include "stdafx.h"

#include "GameObject.h"
#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"
#include <algorithm>

#include "PlayerScript.h"

#include "AirplaneScript.h"
#include "CatmullRomSpline.h"
#include "ExplosiveScript.h"

#include "BulletScript.h"
#include "TankMonster.h"
#include "GameManager.h"


CScene::CScene()
{
}

CScene::~CScene()
{
	for (CGameObject* pObj : m_vecObjects) {
		if (pObj)
		{
			delete pObj;
		}
	}
}

void CScene::Init()
{
	AddFloorObject();

	AddTankObject();

	AddAirplaneObject();

	AddTankMonsterObject();
	GameManager::GetInst()->AddMonsterMaxCnt(1);


	ExplosiveObject();

	for(int i = 0 ; i <10 ; ++i)
		AddBoxObject();

	AddMainCamera();
}

void CScene::Update(float _fTimeElapsed)
{

	for (auto pGameObj : m_vecObjects) {
		if(pGameObj->IsActive())
			pGameObj->Update(_fTimeElapsed);
	}


	CheckObjectByObjectCollisions();
	CheckObjectByBulletCollisions();		/// MONSTER <-> PLAYER'S BULLET
	CheckObjectByMonsterBulletCollisions(); /// PLAYER  <-> MONSTER'S BULLET 

}

void CScene::FinalUpdate(float _fTimeElapsed)
{
	for (auto pGameObj : m_vecObjects) {
		if (pGameObj->IsActive())
			pGameObj->FinalUpdate(_fTimeElapsed);
	}
}

void CScene::Render(HDC FrameBuffer, float _fTimeElapsed)
{
	for (auto pGameObj : m_vecObjects) {
		if(pGameObj->IsActive() && pGameObj->IsRenderObj() )
		pGameObj->Render(FrameBuffer, _fTimeElapsed);

	}

}

void CScene::AddObject(CGameObject* pObj)
{
	m_vecObjects.push_back(pObj);

}

CGameObject* CScene::GetObjByName(std::string name)
{
	
	for (const auto pObj : m_vecObjects) {
		std::vector<CGameObject*> vecChild = pObj->GetAllChildInfo();
		for (const auto pChild : vecChild) {
			CGameObject* pResult = pChild->FindChild(name);
			if (pResult != nullptr)
				return pResult;

			
		}

		if (pObj->GetName() == name)
			return pObj;
	}

	return nullptr;
}

void CScene::AddTankObject()
{
// BODY 
	CGameObject* pBody   = new CGameObject;
	pBody->SetName("TankObject");
	CCubeMesh* pBodyMesh = new CCubeMesh(6.0f, 2.0f, 6.0f);
	pBody->SetMesh(pBodyMesh);
	pBody->SetColor(COLOR::DARK_OLIVE_GREEN);
	pBody->SetPenStyle(PS_SOLID);
	pBody->SetPenWidth(3.f);
	CTransform* pBodyTransform = new CTransform;
	pBodyTransform->SetPosition(0.f, 1.f, -50.f);
	pBody->AddComponent(pBodyTransform);
	PlayerScript* pPlayerSC = new PlayerScript;
	pBodyTransform->IgnoreParentRotation(true);
	pBody->AddScript(pPlayerSC);


// TURRET 
	CGameObject* pTurret = new CGameObject;
	pTurret->SetName("Turret");
	CCubeMesh* pTurretMesh = new CCubeMesh(4.0f, 1.5f, 4.0f);
	pTurret->SetMesh(pTurretMesh);
	pTurret->SetColor(COLOR::DARK_OLIVE_GREEN);
	pTurret->SetPenWidth(2.5f);
	CTransform* pTurretTransform = new CTransform;
	pTurretTransform->SetPosition(0.0f, 1.75f, 0.f);
	pTurretTransform->IgnoreParentRotation(true);

	pTurret->AddComponent(pTurretTransform);

	pBody->AddChild(pTurret);

// GUN
	CGameObject* pGun = new CGameObject;
	pGun->SetName("Gun");
	CGunMesh* pGunMesh = new CGunMesh(1.0f, 1.0f, 10.0f);
	pGun->SetMesh(pGunMesh);
	pGun->SetColor(COLOR::NAVY);
	pGun->SetPenWidth(2.f);
	CTransform* pGunTransform = new CTransform;
	pGunTransform->SetPosition(0.0f, 0.f, 0.f);
	pGunTransform->SetOffset(XMFLOAT3(0.f, 0.f, 0.f));
	pGunTransform->IgnoreParentRotation(true);
	pGun->AddComponent(pGunTransform);


	ExplosiveScript* pExplosiveScript = new ExplosiveScript(pBody);
	pExplosiveScript->SetMesh(pBodyMesh);
	ExplosiveScript::PrepareExplosion();
	pExplosiveScript->Init();
	pExplosiveScript->SetMaxHit(50);

	pBody->AddScript(pExplosiveScript);


	pTurret->AddChild(pGun);

// START
	pPlayerSC->Init();

// ADD Object in Scene
	this->AddObject(pBody);
}

void CScene::AddFloorObject()
{
	float fHalfWidth = 45.0f, fHalfHeight = 45.0f, fHalfDepth = 200.0f;
	CFloorMesh* pFloorMesh = new CFloorMesh(fHalfWidth * 2.0f, fHalfDepth * 2.0f, 30);


	CGameObject* pFloorObject = new CGameObject;
	pFloorObject->SetName("Floor");
	pFloorObject->SetMesh(pFloorMesh);
	pFloorObject->SetPenStyle(PS_SOLID);
	pFloorObject->SetColor(COLOR::BLACK);

	CTransform* pTrans = new CTransform;
	pTrans->SetPosition(0.0f, 0.0f, 0.0f);
	pFloorObject->AddComponent(pTrans);

	this->AddObject(pFloorObject);

}

void CScene::AddMainCamera()
{
	// Main Camera
	m_pMainCamera = new CGameObject;
	m_pMainCamera->SetName("Camera");
	CTransform* pTransform = new CTransform;
	m_pMainCamera->AddComponent(pTransform);

	CCamera* pCamera = new CCamera();
	m_pMainCamera->AddComponent(pCamera);

	pCamera->SetViewport(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	pCamera->GeneratePerspectiveProjectionMatrix(1.01f, 500.0f, 60.0f);
	pCamera->SetFOVAngle(60.0f);
	pCamera->GenerateOrthographicProjectionMatrix(1.01f, 50.0f, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);


	CGameObject* pTargetObject = GetObjByName("TankObject");
	pCamera->SetTargetObject(pTargetObject);
	XMFLOAT3 CamOffSet = XMFLOAT3(0.0f, 5.0f, -15.0f);
	pCamera->SetCameraOffset(CamOffSet);

	this->AddObject(m_pMainCamera);


}

void CScene::AddAirplaneObject()
{
	CGameObject* pObj = new CGameObject;
	pObj->SetName("Airplane");
	CAirplaneMesh* pMesh = new CAirplaneMesh(10.f, 10.f, 2.f);
	pObj->SetMesh(pMesh);
	pObj->SetColor(COLOR::BLACK);
	pObj->SetPenWidth(2.5f);
	CTransform* pTrans = new CTransform;
	pTrans->SetPosition(0.0f, 0.f, 0.f);
	pTrans->SetRotation(90.f, 180.f, 0.f);
	pObj->AddComponent(pTrans);
	
	AirplaneScript* pScript = new AirplaneScript;

	CatmullRomSpline CatmulRom;

	XMFLOAT3 p0{-700.f, 15.f, 1000.f};
	XMFLOAT3 p1{0.f, 15.f, 200.f};
	XMFLOAT3 p2{0.f, 15.f, -300.f};
	XMFLOAT3 p3{ 600.f, 15.f, -1000.f };

	CatmulRom.CreateRoute(p0, p1, p2, p3);

	pScript->Init(CatmulRom);
	pScript->Start();
	pObj->AddScript(pScript);

	this->AddObject(pObj);

}

void CScene::ExplosiveObject()
{
	CGameObject* pObj = new CGameObject;
	pObj->SetName("Explosive");
	CCubeMesh* pMesh = new CCubeMesh(5.f, 5.f, 5.f);
	pObj->SetMesh(pMesh);
	pObj->SetColor(COLOR::YELLOW);
	pObj->SetPenWidth(2.5f);

	pObj->AllowRenderDirect();

	CTransform* pTrans = new CTransform;
	pTrans->SetPosition(0.0f, 0.f, 100.f);
	pObj->AddComponent(pTrans);

	ExplosiveScript* pScript = new ExplosiveScript(pObj);
	pScript->SetMesh(pMesh);
	pScript->BlowStart();
	ExplosiveScript::PrepareExplosion();
	pScript->Init();

	pObj->AddScript(pScript);


	this->AddObject(pObj);

}

void CScene::AddTankMonsterObject()
{

// BODY 
	CGameObject* pBody = new CGameObject;
	pBody->SetName("TankMonster");

	CCubeMesh* pBodyMesh = new CCubeMesh(6.0f, 2.0f, 6.0f);
	pBody->SetMesh(pBodyMesh);
	pBody->SetColor(COLOR::DEEP_RED);
	pBody->SetPenStyle(PS_SOLID);
	pBody->SetPenWidth(3.f);

	CTransform* pBodyTransform = new CTransform;
	pBodyTransform->SetPosition(0.f, 1.f, 0.f);
	pBody->AddComponent(pBodyTransform);
	pBodyTransform->IgnoreParentRotation(true);


// TURRET 
	CGameObject* pTurret = new CGameObject;
	pTurret->SetName("TurretMonster");

	CCubeMesh* pTurretMesh = new CCubeMesh(4.0f, 1.5f, 4.0f);
	pTurret->SetMesh(pTurretMesh);
	pTurret->SetColor(COLOR::DEEP_RED);
	pTurret->SetPenWidth(2.5f);

	CTransform* pTurretTransform = new CTransform;
	pTurretTransform->SetPosition(0.0f, 1.75f, 0.f);
	pTurretTransform->IgnoreParentRotation(true);
	pTurret->AddComponent(pTurretTransform);

	pBody->AddChild(pTurret);

// GUN
	CGameObject* pGun = new CGameObject;
	pGun->SetName("GunMontser");
	CGunMesh* pGunMesh = new CGunMesh(1.0f, 1.0f, 10.0f);
	pGun->SetMesh(pGunMesh);
	pGun->SetColor(COLOR::DEEP_GREY);
	pGun->SetPenWidth(2.f);
	CTransform* pGunTransform = new CTransform;
	pGunTransform->SetPosition(0.0f, 0.f, 0.f);
	pGunTransform->SetOffset(XMFLOAT3(0.f, 0.f, 0.f));
	pGunTransform->IgnoreParentRotation(true);
	pGun->AddComponent(pGunTransform);

	pTurret->AddChild(pGun);

	ExplosiveScript* pScript = new ExplosiveScript(pBody);
	pScript->SetMesh(pBodyMesh);
	pScript->BlowStart();
	ExplosiveScript::PrepareExplosion();
	pScript->Init();
	pScript->SetMaxHit(30);

	TankMonsterScript* pMonScript = new TankMonsterScript;
	TankMonsterScript* pMonScript_Gun = new TankMonsterScript;
	
	pGun->AddScript(pMonScript_Gun);
	pBody->AddScript(pMonScript);
	pBody->AddScript(pScript);

	// ADD Object in Scene
	this->AddObject(pBody);
	pMonScript->Init();
	pMonScript_Gun->Init();


	m_vecMonsters.push_back(pBody);

}

void CScene::AddBoxObject()
{
	CGameObject* pObj = new CGameObject;
	pObj->SetName("Box");
	float BoxSize = RandF(2.f, 7.f);
	CCubeMesh* pMesh = new CCubeMesh(BoxSize, BoxSize, BoxSize);
	pObj->SetMesh(pMesh);
	pObj->SetColor(COLOR::BROWN);
	pObj->SetPenWidth(4.f);

	CTransform* pTrans = new CTransform;
	pTrans->SetPosition(RandF(-50.f, 50.f), RandF(2.f, 10.f), RandF(-100.f, 200.f));
	pObj->AddComponent(pTrans);

	ExplosiveScript* pScript = new ExplosiveScript(pObj);
	pScript->SetMesh(pMesh);
	pScript->BlowStart();
	ExplosiveScript::PrepareExplosion();
	pScript->Init();
	pScript->SetObjType(OBJ_TYPE::BOX);

	pObj->AddScript(pScript);


	this->AddObject(pObj);

}

void CScene::MakeMainStage()
{


}

CGameObject* CScene::GetMainCamera()
{
	if (m_pMainCamera)
		return m_pMainCamera;
	else
		return nullptr;

}

void CScene::SetMainCamera(CGameObject* pObj)
{

}


void CScene::CheckObjectByObjectCollisions()
{
	
}

void CScene::CheckObjectByBulletCollisions()
{
	CGameObject* pTank = this->GetObjByName("TankObject");
	PlayerScript* pTankScript = (PlayerScript*)pTank->GetScript(SCRIPT_TYPE::PLAYER);
	std::vector<CGameObject*> pBullets = pTankScript->GetBullets();

	for (CGameObject* Obj : m_vecObjects) {
		if (Obj == pTank) continue;
		if (Obj->GetName() == "Bullet") continue;
		if (Obj->GetName() == "Floor") continue;
		ExplosiveScript* pObjExplosive = (ExplosiveScript*)Obj->GetScript(SCRIPT_TYPE::EXPLOSIVE);

		for (CGameObject* bullet : pBullets) {
	/*		if (bullet->IsActive() && Obj->IsActive() 
				&& bullet->GetBoundingBox().Intersects(Obj->GetBoundingBox())) 
	*/			if (bullet->IsActive() && Obj->IsActive() && Obj->MyCollideCheck(bullet)){
				ExplosiveScript* pExplosiveScript = (ExplosiveScript*)bullet->GetScript(SCRIPT_TYPE::EXPLOSIVE);
				BulletScript* pBulletScript = (BulletScript*)bullet->GetScript(SCRIPT_TYPE::BULLET);


				if (pExplosiveScript) {
					if (pExplosiveScript->IsBlow() == false)
					{
						if (pBulletScript->IsStart() == false) {
								pBulletScript->Stop();

						if (pObjExplosive)
							pObjExplosive->Hit();

						bullet->AllowRenderDirect();
						pExplosiveScript->BlowStart();
						}
					
					}
				}


			}
		}
	}
}

void CScene::CheckObjectByMonsterBulletCollisions()
{
	CGameObject* pTank = this->GetObjByName("TankObject");

	CGameObject* pTankMonster                 = this->GetObjByName("TankMonster");
	TankMonsterScript* pTankScript            = (TankMonsterScript*)pTankMonster->GetScript(SCRIPT_TYPE::TANK_MONSTER);
	std::vector<CGameObject*> pBullets        = pTankScript->GetBullets();

	ExplosiveScript* pObjExplosive = (ExplosiveScript*)pTank->GetScript(SCRIPT_TYPE::EXPLOSIVE);

		for (CGameObject* bullet : pBullets) {
			/*		if (bullet->IsActive() && Obj->IsActive()
						&& bullet->GetBoundingBox().Intersects(Obj->GetBoundingBox()))
			*/			
				if (bullet->IsActive() && pTank->IsActive() && pTank->MyCollideCheck(bullet)) {
				ExplosiveScript* pExplosiveScript = (ExplosiveScript*)bullet->GetScript(SCRIPT_TYPE::EXPLOSIVE);
				BulletScript* pBulletScript       = (BulletScript*)bullet->GetScript(SCRIPT_TYPE::BULLET);


				if (pExplosiveScript) {
					if (pExplosiveScript->IsBlow() == false)
					{
						if (pBulletScript->IsStart() == false) {
							pBulletScript->Stop();

							if (pObjExplosive)
								pObjExplosive->Hit();

							bullet->AllowRenderDirect();
							pExplosiveScript->BlowStart();
						}
						
					}
				}


			}
		}


}

