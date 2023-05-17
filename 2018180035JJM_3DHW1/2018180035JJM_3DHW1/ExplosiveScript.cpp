#include "stdafx.h"

#include "ExplosiveScript.h"
#include "GameObject.h"
#include"Mesh.h"
#include "Transform.h"
#include "Script.h"
#include "MyUtility.h"

#include "Key.h"
#include"BulletScript.h"
#include "GameManager.h"


XMFLOAT3 ExplosiveScript::m_pxmf3SphereVectors[EXPLOSION_DEBRISES];
CMesh* ExplosiveScript::m_pExplosive_Mesh{ nullptr };


void ExplosiveScript::PrepareExplosion()
{

	for (int i = 0; i < EXPLOSION_DEBRISES; i++)
	{
		XMVECTOR res = RandomUnitVectorOnSphere();
		XMStoreFloat3(&m_pxmf3SphereVectors[i], res);


	}

}

void ExplosiveScript::SetMesh(CMesh* pMesh)
{
	m_pExplosive_Mesh = pMesh;
}

void ExplosiveScript::Reset()
{

	m_fElapsedTimes = 0.0f;

}

void ExplosiveScript::Hit()
{

	m_iHit += 1;
	if (m_iHit >= m_iMaxHit)
	{
		if(m_eMyType == OBJ_TYPE::MONSTER)
			GameManager::GetInst()->AddDieMonster();
		
		GetOwner()->Deactivate();
		HitReset();
	}
	

}

ExplosiveScript::ExplosiveScript()
{
	m_eType = SCRIPT_TYPE::EXPLOSIVE;
}

ExplosiveScript::ExplosiveScript(CGameObject* pOwner)
{
	m_pOwner = pOwner;
	m_eType = SCRIPT_TYPE::EXPLOSIVE;

}

ExplosiveScript::~ExplosiveScript()
{
}

void ExplosiveScript::Init()
{
	for (int i = 0; i < EXPLOSION_DEBRISES; i++)
	{
	CGameObject pObj;
	m_vecExplosiveObjs.push_back(pObj);

	}

	CCubeMesh* pMesh = new CCubeMesh(0.5f, 0.5f, 0.5f);
	GetOwner()->RegisterMesh_RenderDirect(pMesh);

}

void ExplosiveScript::Update(float _fTimeElapsed)
{
	if (m_bBlowingUp)
	{
		GetOwner()->Clear_WorldRenderDirect();
		m_fElapsedTimes += _fTimeElapsed;
		if (m_fElapsedTimes <= m_fDuration)
		{
			CTransform* pTrans = (CTransform*)CScript::GetOwner()->GetComponent(COMPONENT_TYPE::TRANSFORM);
			XMFLOAT3 xmf3Position = pTrans->GetCurPosition();
			for (int i = 0; i < EXPLOSION_DEBRISES; i++)
			{
				m_pxmf4x4Transforms[i] = Matrix4x4::Identity();
				m_pxmf4x4Transforms[i]._41 = xmf3Position.x + m_pxmf3SphereVectors[i].x * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i]._42 = xmf3Position.y + m_pxmf3SphereVectors[i].y * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i]._43 = xmf3Position.z + m_pxmf3SphereVectors[i].z * m_fExplosionSpeed * m_fElapsedTimes;

				XMFLOAT4X4 result = Matrix4x4::RotationAxis(m_pxmf3SphereVectors[i], m_fExplosionRotation * m_fElapsedTimes);
				m_pxmf4x4Transforms[i] = Matrix4x4::Multiply(result,  m_pxmf4x4Transforms[i]);

				
				GetOwner()->RegisterWorld_RenderDirect(m_pxmf4x4Transforms[i]);
			}
		}
		else
		{
 			m_bBlowingUp = false;
			m_fElapsedTimes = 0.0f;
		}
	}

}

void ExplosiveScript::Start()
{
}
