#pragma once
#include "Script.h"

class CGameObject;
class CTransform;

class BulletScript 
    : public CScript
{
private:
	bool		m_bFire;
	bool		m_bEnd;
	float		m_fEndTime = 4.f;
    float       m_fSpeed = 70.f;
    float       m_fTimer = 0.f;

    XMFLOAT3    m_xmf3Dir;
    CTransform* m_pTransform_Com;

    bool						m_bStop = false;

public:
    void Stop() { m_bStop = true; }
    void Start() { m_bStop = false; }
    bool IsStart() { return m_bStop; }
public:
    BulletScript();
    BulletScript(CGameObject* pOwner);
    virtual ~BulletScript() override;

    virtual void Init() override;
    virtual void Update(float _fTimeElapsed) override;

public:
    void Reset();
    void SetTimer(float _fTime) { m_fEndTime = _fTime; }
    void SetTransformCom(CTransform* pTrans) { m_pTransform_Com = pTrans; }
    void SetDir(XMFLOAT3 _xmf3Dir) { m_xmf3Dir = _xmf3Dir; }
    void SetSpeed(float _fSpeed) { m_fSpeed = _fSpeed; }
};

