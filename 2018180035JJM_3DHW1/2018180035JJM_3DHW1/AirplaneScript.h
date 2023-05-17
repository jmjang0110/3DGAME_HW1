#pragma once
#include "Script.h"
#include "CatmullRomSpline.h"

class AirplaneScript :
    public CScript
{
private:
    std::vector<CGameObject*>   m_pBullets;
    UINT                        m_iBulletMax = 50;

    CatmullRomSpline            m_CatmullRom;

public:
    AirplaneScript();
    AirplaneScript(CGameObject* pOwner);
    virtual ~AirplaneScript() override;

    virtual void Init(CatmullRomSpline _Catmul) { m_CatmullRom = _Catmul; }
    virtual void Update(float _fTimeElapsed) override;


    void Start();

    void RotateFront();

};

