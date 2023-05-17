#include "Component.h"
#include "stdafx.h"

CComponent::CComponent() 
{
	m_pOwner = nullptr;
	m_bActive = true;

}

CComponent::~CComponent()
{
}

void CComponent::Init()
{
}

void CComponent::Update(float _fTimeElapsed)
{
}

void CComponent::FinalUpdate(float _fTimeElapsed)
{
}

void CComponent::Activate()
{
}

void CComponent::Deactivate()
{
}

bool CComponent::IsActive()
{
	return m_bActive;
}

COMPONENT_TYPE CComponent::GetComponentType()
{

	return m_eType;

}
