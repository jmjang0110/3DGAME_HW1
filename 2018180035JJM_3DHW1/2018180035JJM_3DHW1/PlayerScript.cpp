#include "PlayerScript.h"
#include "GameObject.h"

#include "Script.h"
#include "Timer.h"
#include "Transform.h"
#include "Key.h"

#include "Mesh.h"
#include "GameFramework.h"
#include "Scene.h"

#include "BulletScript.h"
#include "Camera.h"
#include "ExplosiveScript.h"


PlayerScript::PlayerScript()
{
	CScript::m_eType = SCRIPT_TYPE::PLAYER;

}

PlayerScript::PlayerScript(CGameObject* pOwner) :
	CScript(pOwner)
{
	CScript::m_eType = SCRIPT_TYPE::PLAYER;

}

PlayerScript::~PlayerScript()
{


}

void PlayerScript::Init()
{
	m_pTurret = CScript::GetOwner()->FindChild("Turret");
	m_pGun    = CScript::GetOwner()->FindChild("Gun");


	for (UINT i = 0; i < m_iBulletMax; ++i) {

		CGameObject* pBullet = new CGameObject;
		pBullet->SetName("Bullet");
		CCubeMesh* pMesh = new CCubeMesh(0.7f, 0.7f, 0.7f);
		pBullet->SetMesh(pMesh);
		pBullet->SetColor(COLOR::SKY_BLUE);
		pBullet->SetPenWidth(2.f);
		CTransform* pTransCom = new CTransform;
		pBullet->AddComponent(pTransCom);

		BulletScript* pScript = new BulletScript;
		pBullet->AddScript(pScript);
		pScript->Init();

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

void PlayerScript::Update(float _fTimeElapsed)
{

	Update_InputKey(_fTimeElapsed);

	Rotate_PlayerAxis_Sync_Camera_Axis(_fTimeElapsed);

}

void PlayerScript::UpdateCamera_ThirdPersonMode(float _fTimeElapsed)
{

	if (!m_pMainCam)
		m_pMainCam = m_pCurScene->GetMainCamera();
	CTransform* pCamTrans    = (CTransform*)m_pMainCam->GetComponent(COMPONENT_TYPE::TRANSFORM);
	XMFLOAT3	vRot      = pCamTrans->GetRotation();
	const XMFLOAT2 vMouseDir = Key::GetInst()->GetMouseDir();

	
	if (KEY_PRESSED(KEY::RBTN))
	{
		vRot = pCamTrans->ToRadian(vRot);
		
		vRot.y += _fTimeElapsed * vMouseDir.x * XM_PI;
		vRot.x -= _fTimeElapsed * vMouseDir.y * XM_PI;

		vRot = pCamTrans->ToDegree(vRot);
		
		pCamTrans->SetRotation(vRot);
	}


// CAMERA 
	CTransform* pCamTransform = (CTransform*)m_pMainCam->GetComponent(COMPONENT_TYPE::TRANSFORM);
	CCamera* pCamCamera       = (CCamera*)m_pMainCam->GetComponent(COMPONENT_TYPE::CAMERA);
	XMFLOAT3	xmf3Campos    = pCamTransform->GetCurPosition();

// TARGET
	CGameObject* pTargetObj     = CScript::GetOwner(); // Player
	CTransform*  pTargetTrans   = (CTransform*)pTargetObj->GetComponent(COMPONENT_TYPE::TRANSFORM);
	AXIS		 tTragetObjAxis = pTargetTrans->GetAxis();
	XMFLOAT3	 vTargetpos     = pTargetTrans->GetCurPosition();


// CAMERA MOVE 
	XMFLOAT3 xmf3Direction             = Vector3::Subtract(vTargetpos, xmf3Campos);
	float fLength                      = Vector3::Length(xmf3Direction);
	
	XMFLOAT3 vBackWard = pCamTrans->GetWorldDir(DIR_TYPE::BACKWARD);
	
	xmf3Campos.x       = vTargetpos.x + (vBackWard.x * m_fMaxOffsetDistance);
	xmf3Campos.y       = m_fOffsetHeight + vTargetpos.y + (vBackWard.y * m_fMaxOffsetDistance);
	xmf3Campos.z       = vTargetpos.z + (vBackWard.z * m_fMaxOffsetDistance);
	
	XMFLOAT3 vUp		= pTargetTrans->GetWorldDir(DIR_TYPE::UP);

	
//CAMERA LOOK AT / POSITION SETTING 
	pCamCamera->SetLookAt(xmf3Campos, vTargetpos, vUp);
	pCamTransform->SetPosition(xmf3Campos);

}

void PlayerScript::Rotate_PlayerAxis_Sync_Camera_Axis(float _fTimeElapsed)
{
	CTransform* pCamTrans    = (CTransform*)m_pMainCam->GetComponent(COMPONENT_TYPE::TRANSFORM);
	CTransform* pPlayerTrans = (CTransform*)GetOwner()->GetComponent(COMPONENT_TYPE::TRANSFORM);

	XMFLOAT3 vCamFront    = pCamTrans->GetWorldDir(DIR_TYPE::FRONT);
	XMFLOAT3 vPlayerFront = pPlayerTrans->GetWorldDir(DIR_TYPE::FRONT);

	RotatePlayer(vCamFront, vPlayerFront, _fTimeElapsed);
}
float PlayerScript::GetRadAngle(XMFLOAT3 _Standard, XMFLOAT3 _Start) {
	
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

	double fRotRadian = acos(fCos);
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


void PlayerScript::RotatePlayer(XMFLOAT3 vEndDir, XMFLOAT3 vStartDir, float _fTimeElapsed)
{
	CTransform* pCamTrans = (CTransform*)m_pMainCam->GetComponent(COMPONENT_TYPE::TRANSFORM);
	XMFLOAT3 vCamRot = pCamTrans->GetRotation();

	CTransform* pTrans = (CTransform*)GetOwner()->GetComponent(COMPONENT_TYPE::TRANSFORM);
	XMFLOAT3 vRot      = pTrans->GetRotation();
	vRot               = pTrans->ToRadian(vRot);

	XMFLOAT3 Edir = vEndDir;
	XMFLOAT3 Sdir = vStartDir;

	// 플레이어가 돌아야할 남은 각도 
	float	fRadAngle	= GetRadAngle(Edir, Sdir);
	int		iRotDir		= (fRadAngle <= 0.f) ? -1 : 1;
	
	// 회전 시작 !
	if (m_iRotateDir_Past_ForCamAxis == 0) {
		if(m_xmf3_PastCamRot.y != vCamRot.y)
			m_iRotateDir_Past_ForCamAxis = iRotDir;
	}
	// 회전 끝 
	else if (m_iRotateDir_Past_ForCamAxis != iRotDir) {
		m_iRotateDir_Past_ForCamAxis = 0;
		m_xmf3_PastCamRot = vCamRot;

	}
	// 회전 중 
	else
	{
		if (m_xmf3_PastCamRot.y != vCamRot.y)
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

	m_iRotateDir_Past_ForCamAxis = iRotDir;
}

bool PlayerScript::CheckRotateFinish(float _fRot_DT)
{
	float fDT = (_fRot_DT > 0.f) ? -1 * _fRot_DT : 1 * _fRot_DT;
	m_fMoveAccRot += fDT;
	if (m_fMoveAccRot <= 0.f)
	{
		m_fMoveAccRot = 0.f;
		return true;
	}
	else
		return false;
}


void PlayerScript::Update_InputKey(float _fTimeElapsed)
{
	if (!m_pMainCam)
		m_pMainCam = m_pCurScene->GetMainCamera();

	CTransform* pTrans    = (CTransform*)CScript::GetOwner()->GetComponent(COMPONENT_TYPE::TRANSFORM);
	XMFLOAT3 vCurPos      = pTrans->GetCurPosition();
	CTransform* pCamTrans = (CTransform*)m_pMainCam->GetComponent(COMPONENT_TYPE::TRANSFORM);

	UpdateCamera_ThirdPersonMode(_fTimeElapsed);

	if (KEY_PRESSED(m_eFrontKey)) {
		XMFLOAT3 vDir = pCamTrans->GetWorldDir(DIR_TYPE::FRONT);
		
		vCurPos.x += _fTimeElapsed * 10.f * vDir.x;
		//vCurPos.y += _fTimeElapsed * 10.f * vFront.y;
		vCurPos.z += _fTimeElapsed * 10.f * vDir.z;


	}
	if (KEY_PRESSED(m_eBackKey)) {
		XMFLOAT3 vDir = pCamTrans->GetWorldDir(DIR_TYPE::BACKWARD);


		vCurPos.x += _fTimeElapsed * 10.f * vDir.x;
		//vCurPos.y += _fTimeElapsed * 10.f * vFront.y;
		vCurPos.z += _fTimeElapsed * 10.f * vDir.z;
	}

	if (KEY_PRESSED(m_eLeftKey)) {
		XMFLOAT3 vDir = pCamTrans->GetWorldDir(DIR_TYPE::LEFT);


		vCurPos.x += _fTimeElapsed * 10.f * vDir.x;
		//vCurPos.y += _fTimeElapsed * 10.f * vFront.y;
		vCurPos.z += _fTimeElapsed * 10.f * vDir.z;
	}

	if (KEY_PRESSED(m_eRightKey)) {
		XMFLOAT3 vDir = pCamTrans->GetWorldDir(DIR_TYPE::RIGHT);


		vCurPos.x += _fTimeElapsed * 10.f * vDir.x;
		//vCurPos.y += _fTimeElapsed * 10.f * vFront.y;
		vCurPos.z += _fTimeElapsed * 10.f * vDir.z;
	}


	if (KEY_AWAY(m_eAttackKey)) {
		for (auto pBullet : m_pBullets) {
			if (pBullet->IsActive() == false)
			{
				pBullet->DenyRenderDirect();
				CTransform*		pBulletTrans = (CTransform*)pBullet->GetComponent(COMPONENT_TYPE::TRANSFORM);
				BulletScript*	pScript      = (BulletScript*)pBullet->GetScript(SCRIPT_TYPE::BULLET);
				
				CTransform*		pGunTrans    = (CTransform*)m_pGun->GetComponent(COMPONENT_TYPE::TRANSFORM);
				
	

				XMFLOAT3 BulletStartPos = vCurPos;
				BulletStartPos.y       += 2.f;
				XMFLOAT3 vFront         = pGunTrans->GetWorldDir(DIR_TYPE::FRONT);
				BulletStartPos.x       += vFront.x * 10.f;
				BulletStartPos.y       += vFront.y * 10.f;
				BulletStartPos.z       += vFront.z * 10.f;


				pBulletTrans->SetPosition(BulletStartPos);


			    XMFLOAT3 vForwardDir = pGunTrans->GetWorldDir(DIR_TYPE::FRONT);

				pScript->SetDir(vForwardDir);
				pBullet->Activate();
				pScript->Start();

				break;
			}
		}
	}

	if (KEY_PRESSED(m_eTurnLeftKey)) {
		CTransform* pGunTrans = (CTransform*)m_pGun->GetComponent(COMPONENT_TYPE::TRANSFORM);
		XMFLOAT3 GunRot       = pGunTrans->GetRotation();
		GunRot.y             -= m_fGunRotation_Speed;
		pGunTrans->SetRotation(GunRot);


		CTransform* pTurretTrans = (CTransform*)m_pTurret->GetComponent(COMPONENT_TYPE::TRANSFORM);
		XMFLOAT3 TurRot = pTurretTrans->GetRotation();
		TurRot.y -= m_fGunRotation_Speed;
		pTurretTrans->SetRotation(TurRot);


	}
	if (KEY_PRESSED(m_eTurnRightKey)) {
		CTransform* pGunTrans = (CTransform*)m_pGun->GetComponent(COMPONENT_TYPE::TRANSFORM);
		XMFLOAT3 GunRot       = pGunTrans->GetRotation();
		GunRot.y             += m_fGunRotation_Speed;
		pGunTrans->SetRotation(GunRot);

		CTransform* pTurretTrans = (CTransform*)m_pTurret->GetComponent(COMPONENT_TYPE::TRANSFORM);
		XMFLOAT3 TurRot = pTurretTrans->GetRotation();
		TurRot.y += m_fGunRotation_Speed;
		pTurretTrans->SetRotation(TurRot);

	}

	if (KEY_PRESSED(m_eTurnUpKey)) {
		CTransform* pGunTrans = (CTransform*)m_pGun->GetComponent(COMPONENT_TYPE::TRANSFORM);
		XMFLOAT3 GunRot       = pGunTrans->GetRotation();
		GunRot.x             -= m_fGunRotation_Speed;
		pGunTrans->SetRotation(GunRot);

	}

	if (KEY_PRESSED(m_eTurnDownKey)) {
		CTransform* pGunTrans = (CTransform*)m_pGun->GetComponent(COMPONENT_TYPE::TRANSFORM);
		XMFLOAT3 GunRot       = pGunTrans->GetRotation();
		GunRot.x             += m_fGunRotation_Speed;
		pGunTrans->SetRotation(GunRot);

	}


	pTrans->SetPosition(vCurPos);
}
