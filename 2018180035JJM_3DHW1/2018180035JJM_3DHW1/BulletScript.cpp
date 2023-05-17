
#include "stdafx.h"
#include "BulletScript.h"

#include "Transform.h"

BulletScript::BulletScript()
{
	CScript::m_eType = SCRIPT_TYPE::BULLET;

}

BulletScript::BulletScript(CGameObject* pOwner) :
	CScript(pOwner)
	
{
	CScript::m_eType = SCRIPT_TYPE::BULLET;

}

BulletScript::~BulletScript()
{

}

void BulletScript::Init()
{
	m_pTransform_Com = (CTransform*)CScript::GetOwner()->GetComponent(COMPONENT_TYPE::TRANSFORM);


}

void BulletScript::Update(float _fTimeElapsed)
{
	if (m_bEnd == true) {
		Reset();
		CScript::GetOwner()->Deactivate();
	}

	if (!m_bStop)
	{
		XMFLOAT3 CurPos = m_pTransform_Com->GetCurPosition();

		CurPos.x += m_xmf3Dir.x * _fTimeElapsed * m_fSpeed;
		CurPos.y += m_xmf3Dir.y * _fTimeElapsed * m_fSpeed;
		CurPos.z += m_xmf3Dir.z * _fTimeElapsed * m_fSpeed;

		m_pTransform_Com->SetPosition(CurPos);
	}
	

	m_fTimer += _fTimeElapsed;
	if (m_fTimer >= m_fEndTime)
		m_bEnd = true;
}

void BulletScript::Reset()
{
	m_bFire = false;
	m_bEnd = false;
	
	m_fTimer = 0.f;

}
