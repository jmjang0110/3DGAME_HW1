#pragma once
#include "Script.h"
class CMesh;
class CGameObject;

enum class OBJ_TYPE {
	MONSTER,
	BOX,
	END,

};
class ExplosiveScript 
	: public CScript
{
private:
	OBJ_TYPE					m_eMyType = OBJ_TYPE::MONSTER;

	int							m_iHit    = 0;
	int							m_iMaxHit = 10;


	float						m_fElapsedTimes = 0.0f;

	float						m_fExplosionSpeed    = 10.0f;
	float						m_fExplosionRotation = 720.0f;
	bool						m_bBlowingUp         = false;


	std::vector<CGameObject>	m_vecExplosiveObjs;
	XMFLOAT4X4					m_pxmf4x4Transforms[EXPLOSION_DEBRISES];
	float						m_fDuration = 1.f;
		

public:
	static XMFLOAT3				m_pxmf3SphereVectors[EXPLOSION_DEBRISES];
	static CMesh*				m_pExplosive_Mesh;
	
	static void PrepareExplosion();
	void SetObjType(OBJ_TYPE _eType) { m_eMyType = _eType; }
public:
	void SetMesh(CMesh* pMesh);
	void BlowStart() { if (!m_bBlowingUp) { Reset(); m_bBlowingUp = true; } }
	void BlowEnd() { m_bBlowingUp = false; }
	void Reset();
	bool IsBlow() { return m_bBlowingUp; }

	void SetMaxHit(int n) { m_iMaxHit = n; }
	int GetHit() { return m_iHit; }

	void Hit();
	void HitReset() { m_iHit = 0; }


	ExplosiveScript();
	ExplosiveScript(CGameObject* pOwner);
	virtual ~ExplosiveScript() override;


	virtual void Init() override;
	virtual void Update(float _fTimeElapsed) override;

	void Start();

	static float RandF(float fMin, float fMax)
	{
		return(fMin + ((float)rand() / (float)RAND_MAX) * (fMax - fMin));
	}


	static XMVECTOR RandomUnitVectorOnSphere()
	{
		XMVECTOR xmvOne = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		XMVECTOR xmvZero = XMVectorZero();

		while (true)
		{
			XMVECTOR v = XMVectorSet(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f)
				, RandF(-1.0f, 1.0f), 0.0f);
			if (!XMVector3Greater(XMVector3LengthSq(v), xmvOne))
				return(XMVector3Normalize(v));
		}
	}

};

