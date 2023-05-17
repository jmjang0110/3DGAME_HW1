
#include "stdafx.h"
#include "Script.h"
#include "AirplaneScript.h"
#include "Transform.h"

AirplaneScript::AirplaneScript()
{
}

AirplaneScript::AirplaneScript(CGameObject* pOwner)
{
}

AirplaneScript::~AirplaneScript()
{
}

void AirplaneScript::Update(float _fTimeElapsed)
{
	m_CatmullRom.Update(_fTimeElapsed);
	XMFLOAT3 fCurPos = m_CatmullRom.CalculatePosByCatmullRom();

	CTransform* pTrans = (CTransform*)CScript::GetOwner()->GetComponent(COMPONENT_TYPE::TRANSFORM);
	pTrans->SetPosition(fCurPos);


}

void AirplaneScript::Start()
{
	m_CatmullRom.m_bStart = true;

}

void AirplaneScript::RotateFront()
{
}
