#pragma once
#include "global.h"
#include "MyUtility.h"

class CGameObject;

class CScript
{
protected:
	SCRIPT_TYPE		m_eType;
	bool			m_bActive = true;
	std::string		m_Name;

	CGameObject*	m_pOwner;

public:
	CScript();
	CScript(CGameObject* pOWner);
	virtual ~CScript();

public:
	void SetName(std::string _name) { m_Name = _name; }

	CGameObject* GetOwner() { return m_pOwner; }
	void SetOWner(CGameObject* pObj) { m_pOwner = pObj; }

	void Activate() { m_bActive = true; }
	void Deactivate() { m_bActive = false; }

	SCRIPT_TYPE GetType() { return m_eType; }
public:
	virtual void Init();
	virtual void Update(float _fTimeElapsed);

};

