#pragma once
#include "MyUtility.h"
#include "GameObject.h"


class CComponent
{
private:

	bool			m_bActive;
	
	COMPONENT_TYPE	m_eType;
	CGameObject*	m_pOwner;

public:
	CComponent();
	virtual ~CComponent();

public:

	virtual void Init();
	virtual void Update(float _fTimeElapsed);
	virtual void FinalUpdate(float _fTimeElapsed);

	
	void Activate();
	void Deactivate();
	bool IsActive();

	CGameObject* GetOwner() { return m_pOwner; }
	void SetOwner(CGameObject* pOwner) { m_pOwner = pOwner; }


	COMPONENT_TYPE GetComponentType();
	void SetComponentType(COMPONENT_TYPE eType) { m_eType = eType; }

	friend class CGameObject;

};

