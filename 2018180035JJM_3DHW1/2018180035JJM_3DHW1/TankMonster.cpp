
#include "stdafx.h"
#include "TankMonster.h"

#include "Timer.h"
#include "Transform.h"
#include "Key.h"

#include "Mesh.h"
#include "GameFramework.h"
#include "Scene.h"

#include "BulletScript.h"
#include "ExplosiveScript.h"
#include "CatmullRomSpline.h"

TankMonsterScript::TankMonsterScript()
{
	m_eType = SCRIPT_TYPE::TANK_MONSTER;


}

TankMonsterScript::TankMonsterScript(CGameObject* pOwner)
{
	m_eType = SCRIPT_TYPE::TANK_MONSTER;


}

TankMonsterScript::~TankMonsterScript()
{
}

void TankMonsterScript::Init()
{
	m_pBody   = CGameFramework::GetInst()->GetCurScene()->GetObjByName("TankMonster");
	m_pTurret = CScript::GetOwner()->FindChild("TurretMonster");
	m_pGun    = CGameFramework::GetInst()->GetCurScene()->GetObjByName("GunMontser");


	for (int i = 0; i < m_iBulletMax; ++i) {

		CGameObject* pBullet = new CGameObject;
		pBullet->SetName("Bullet");
		CCubeMesh* pMesh = new CCubeMesh(0.7f, 0.7f, 0.7f);
		pBullet->SetMesh(pMesh);
		pBullet->SetColor(COLOR::RED);
		pBullet->SetPenWidth(2.f);
		CTransform* pTransCom = new CTransform;
		pBullet->AddComponent(pTransCom);

		BulletScript* pScript = new BulletScript;
		pBullet->AddScript(pScript);
		pScript->Init();
		pScript->SetSpeed(60.f);


		ExplosiveScript* pBulletScript = new ExplosiveScript(pBullet);
		pBulletScript->SetMesh(pMesh);
		//ExplosiveScript::PrepareExplosion();
		pBulletScript->Init();

		pBullet->AddScript(pBulletScript);

		// ** DEACTIVATE **
		pBullet->Deactivate();
		CGameFramework::GetInst()->GetCurScene()->AddObject(pBullet);
		m_pBullets.push_back(pBullet);

	}

	m_pCurScene = CGameFramework::GetInst()->GetCurScene();


}

void TankMonsterScript::Update(float _fTimeElapsed)
{
	if (!m_pTargetObj)
		m_pTargetObj = CGameFramework::GetInst()->GetCurScene()->GetObjByName("TankObject");



	CTransform* pTargetTrans = (CTransform*)m_pTargetObj->GetComponent(COMPONENT_TYPE::TRANSFORM);
	CTransform* pPlayerTrans = (CTransform*)GetOwner()->GetComponent(COMPONENT_TYPE::TRANSFORM);

	XMFLOAT3 vTargetFront = pTargetTrans->GetWorldDir(DIR_TYPE::FRONT);
	XMFLOAT3 vPlayerFront = pPlayerTrans->GetWorldDir(DIR_TYPE::FRONT);

	XMFLOAT3 vTargetPos = pTargetTrans->GetCurPosition();
	XMFLOAT3 vPlayerPOs = pPlayerTrans->GetCurPosition();

	XMFLOAT3 vEnddir    = Vector3::Subtract(vTargetPos, vPlayerPOs);
	vEnddir             = Vector3::Normalize(vEnddir);

	RotateToPlayer(vEnddir, vPlayerFront, _fTimeElapsed);

	if (GetOwner()->GetName() == "TankMonster")
	{
		UpdateMove(_fTimeElapsed);
		m_fTimer += _fTimeElapsed;
		if (m_fTimer >= 2.f)
		{
			m_fTimer = 0.f;
			Fire();
		}
	}

}

void TankMonsterScript::Start()
{
}

void TankMonsterScript::RotateToPlayer(XMFLOAT3 vEndDir, XMFLOAT3 vStartDir, float _fTimeElapsed)
{
	CTransform* pTargetTrans = (CTransform*)m_pTargetObj->GetComponent(COMPONENT_TYPE::TRANSFORM);
	CTransform* pBodyTrans   = (CTransform*)m_pBody->GetComponent(COMPONENT_TYPE::TRANSFORM);
	CTransform* pTrans       = (CTransform*)GetOwner()->GetComponent(COMPONENT_TYPE::TRANSFORM);

	XMFLOAT3 pTargetRot = pTargetTrans->GetRotation();
	XMFLOAT3 vRot       = pBodyTrans->GetRotation();
	vRot                = pBodyTrans->ToRadian(vRot);

	XMFLOAT3 Edir = vEndDir;
	XMFLOAT3 Sdir = vStartDir;

	// 플레이어가 돌아야할 남은 각도 
	float	fRadAngle = GetRadAngle(Edir, Sdir);
	int		iRotDir = (fRadAngle <= 0.f) ? -1 : 1;

	if (iRotDir <= 0) { // CCW
		vRot.y += iRotDir * _fTimeElapsed * 1.f;

	}
	else {				// CW
		vRot.y += iRotDir * _fTimeElapsed * 1.f;

	}

	vRot = pTrans->ToDegree(vRot);
	pTrans->SetRotation(vRot);

	return;

	// 회전 시작 !
	if (m_iRotateDir_ToPastAxis == 0) {
		if (m_xmf3_PastTargetRot.y != fRadAngle)
			m_iRotateDir_ToPastAxis = iRotDir;
	}
	// 회전 끝 
	else if (m_iRotateDir_ToPastAxis != iRotDir) {
		m_iRotateDir_ToPastAxis = 0;
		m_xmf3_PastTargetRot = pTargetRot;

	}
	// 회전 중 
	else
	{
		if (m_xmf3_PastTargetRot.y != fRadAngle)
		{
			if (iRotDir <= 0) { // CCW
				vRot.y += iRotDir * _fTimeElapsed * 1.f;

			}
			else {				// CW
				vRot.y += iRotDir * _fTimeElapsed * 1.f;

			}
		}

	}
	vRot = pTrans->ToDegree(vRot);
	pTrans->SetRotation(vRot);

	m_iRotateDir_ToPastAxis = iRotDir;
}


void TankMonsterScript::UpdateMove(float _fTimeElapsed)
{
	CTransform* ptrans = (CTransform*)m_pBody->GetComponent(COMPONENT_TYPE::TRANSFORM);
	XMFLOAT3 vCurPos   = ptrans->GetCurPosition();

	m_fAcc += _fTimeElapsed * m_iMoveDir;
	if (m_fAcc >= 4)
		m_iMoveDir = -1;
	else if (m_fAcc <= -4)
		m_iMoveDir = 1;

	vCurPos.x += _fTimeElapsed * m_fMoveSpeed * m_iMoveDir;
	ptrans->SetPosition(vCurPos);

}

float TankMonsterScript::GetRadAngle(XMFLOAT3 _Standard, XMFLOAT3 _Start)
{
	XMVECTOR _vA = XMLoadFloat3(&_Standard);
	XMVECTOR _vB = XMLoadFloat3(&_Start);

	if (XMVector3Equal(_vA, _vB))
		return 0.f;
	if (XMVector3Equal(_vA, -1 * _vB))
		return XM_PI;

	// [ -180 ~ 180 Degree ]

		// 카메라가		앞을 보는 벡터와
		// 플레이어가	앞을 보는 벡터 사이의 각을 갱신한다. 

	_vA = XMVector3Normalize(_vA);
	_vB = XMVector3Normalize(_vB);

	// 두 벡터 사이의 내적값은 0 ~ 180 도 사이 값이 나온다. -> 외적값을 이용해 CW/CCW 이동방향인지 확인한다. 
	XMVECTOR X = XMVector3Dot(_vA, _vB);
	float fCos = XMVectorGetX(X);
	if (fCos >= 1.f) fCos = 1.f;
	if (fCos <= -1.f) fCos = -1.f;

	float fRotRadian = acos(fCos);
	float fRotAngle = fRotRadian * (180.f / XM_PI);

	XMVECTOR vResult = XMVector3Cross(_vA, _vB);
	XMFLOAT3 vCross;
	XMStoreFloat3(&vCross, vResult);

	// _vTarget 기준으로 음수 방향으로 돌아야 하는 경우 ( 왼쪽 ) 
	if (vCross.y > 0.f)
	{
		//fRotRadian = acos(_vC.Dot(_vB)) + XM_PI;
		fRotRadian *= -1.f;
		fRotAngle *= -1.f;
	}

	return fRotRadian;
}

void TankMonsterScript::UpdatePosition(float _fTimeElapsed)
{
	if (m_vecCatmullRom.empty())
		return;

	// CatmullRom 에 저장된 위치를 이용해 카메라를 움직인다. 
	for (int i = 0; i < m_vecCatmullRom.size(); ++i)
	{
		if (!m_vecCatmullRom[i].m_bStart)
			continue;
		// CatmullRom[i] 번째 움직임이 끝났다면 다음 움직임으로 전환  
		if (m_vecCatmullRom[i].m_bIsEnd)
		{
			if (i + 1 <= m_vecCatmullRom.size() - 1)
			{
				m_vecCatmullRom[i + 1].m_bStart = true;
				continue;
			}
		}

		m_vecCatmullRom[i].Update(_fTimeElapsed);
		XMFLOAT3 vPos = m_vecCatmullRom[i].CalculatePosByCatmullRom();
		CTransform*  pTrans = (CTransform*)GetOwner()->GetComponent(COMPONENT_TYPE::TRANSFORM);
		pTrans->SetPosition(vPos);

	}


	// [0] - > [1] - > [2] ---...[0] - > ...
	if (m_bLoopRepeat
		&& m_vecCatmullRom[m_vecCatmullRom.size() - 1].m_bIsEnd)
	{
		ResetCatmullRom();
	}
	else if (m_vecCatmullRom[m_vecCatmullRom.size() - 1].m_bIsEnd)
		m_bFinish = true;

}

void TankMonsterScript::ResetCatmullRom()
{
	for (int i = 0; i < m_vecCatmullRom.size(); ++i)
	{
		m_vecCatmullRom[i].m_bStart = false;
		m_vecCatmullRom[i].m_bIsEnd = false;
		m_vecCatmullRom[i].m_fInterPolation_Control_Coefficient = 0.f;
	}

	if (!m_vecCatmullRom.empty())

		m_vecCatmullRom[0].m_bStart = true;
}

void TankMonsterScript::ClearCatmullRom()
{

}

XMFLOAT3 TankMonsterScript::CalCulateCatmullRom(XMFLOAT3 _p0, XMFLOAT3 _p1, XMFLOAT3 _p2, XMFLOAT3 _p3, float _t)
{
	XMFLOAT3 Result;

	float t2 = _t * _t;
	float t3 = t2 * _t;

	float P0 = -t3 + 2.0f * t2 - _t;
	float P1 = 3.0f * t3 - 5.0f * t2 + 2.0f;
	float P2 = -3.0f * t3 + 4.0f * t2 + _t;
	float P3 = t3 - t2;

	Result.x = (P0 * _p0.x + P1 * _p1.x + P2 * _p2.x + P3 * _p3.x) * 0.5f;
	Result.y = (P0 * _p0.y + P1 * _p1.y + P2 * _p2.y + P3 * _p3.y) * 0.5f;
	Result.z = (P0 * _p0.z + P1 * _p1.z + P2 * _p2.z + P3 * _p3.z) * 0.5f;
	//Result.w = (P0 * _p0.w + P1 * _p1.w + P2 * _p2.w + P3 * _p3.w) * 0.5f;

	return Result;
}

void TankMonsterScript::CreateCatmullRomRoute(XMFLOAT3 _vPos0, std::vector<XMFLOAT3> _vecMainPos, XMFLOAT3 _vPos1)
{
}

void TankMonsterScript::CreateStartMap02Route(XMFLOAT3 _vStartPos)
{
	if (m_pTargetObj)
	{
		CTransform* pTargetTrans = (CTransform*)m_pTargetObj->GetComponent(COMPONENT_TYPE::TRANSFORM);

		XMFLOAT3 vPos{};
		XMFLOAT3 vForwardAxis = pTargetTrans->GetWorldDir(DIR_TYPE::FRONT);
		XMFLOAT3 vBackWardAxis = pTargetTrans->GetWorldDir(DIR_TYPE::BACKWARD);;
		XMFLOAT3 vRightAxis = pTargetTrans->GetWorldDir(DIR_TYPE::RIGHT);
		XMFLOAT3 vLeftAxis = pTargetTrans->GetWorldDir(DIR_TYPE::LEFT);;

		//XMFLOAT3 vPlayerPos = m_pTargetObject->Transform()->GetRelativePos();
		XMFLOAT3 vPos0 = _vStartPos;
		XMFLOAT3 vPos1 = _vStartPos;

		std::vector<XMFLOAT3> MainPos;
		// TODO : 채우기 


		CreateCatmullRomRoute(vPos0, MainPos, vPos1);
		m_vecCatmullRom[0].m_bStart = true;
		m_vecCatmullRom[0].SetSpeed(0.6f);
		m_vecCatmullRom[1].SetSpeed(0.7f);
		m_vecCatmullRom[2].SetSpeed(0.7f);
		m_vecCatmullRom[3].SetSpeed(0.6f);
		m_vecCatmullRom[4].SetSpeed(0.5f);
		m_vecCatmullRom[5].SetSpeed(0.2f);

	}
	else
	{
		XMFLOAT3 vPos0 = XMFLOAT3(0.f, 0.f, 0.f);
		XMFLOAT3 vPos1 = XMFLOAT3(1000.f, 1000.f, 1000.f);

		std::vector<XMFLOAT3> mainPos;
		mainPos.push_back(XMFLOAT3(500.f, 500.f, 500.f));
		mainPos.push_back(XMFLOAT3(100.f, 100.f, 100.f));
		mainPos.push_back(XMFLOAT3(-500.f, -500.f, 100.f));

		CreateCatmullRomRoute(vPos0, mainPos, vPos1);
		m_vecCatmullRom[0].m_bStart = true;
	}

}

void TankMonsterScript::Fire()
{
	if (m_pGun == nullptr)
		return;

	CTransform* pTrans    = (CTransform*)CScript::GetOwner()->GetComponent(COMPONENT_TYPE::TRANSFORM);
	XMFLOAT3 vCurPos      = pTrans->GetCurPosition();
	CTransform* pGunTrans = (CTransform*)m_pGun->GetComponent(COMPONENT_TYPE::TRANSFORM);


		for (auto pBullet : m_pBullets) {
			if (pBullet->IsActive() == false)
			{
				pBullet->DenyRenderDirect();
				CTransform* pBulletTrans = (CTransform*)pBullet->GetComponent(COMPONENT_TYPE::TRANSFORM);
				BulletScript* pScript = (BulletScript*)pBullet->GetScript(SCRIPT_TYPE::BULLET);


				XMFLOAT3 BulletStartPos = vCurPos;
				BulletStartPos.y += 2.f;
				XMFLOAT3 vFront = pGunTrans->GetWorldDir(DIR_TYPE::FRONT);
				BulletStartPos.x += vFront.x * 10.f;
				BulletStartPos.y += vFront.y * 10.f;
				BulletStartPos.z += vFront.z * 10.f;


				pBulletTrans->SetPosition(BulletStartPos);


				XMFLOAT3 vForwardDir = pGunTrans->GetWorldDir(DIR_TYPE::FRONT);

				pScript->SetDir(vForwardDir);
				pBullet->Activate();
				pScript->Start();

				break;
			}
		}
	

}
