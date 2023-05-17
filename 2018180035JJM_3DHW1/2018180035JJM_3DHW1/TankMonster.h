#pragma once
#include "Script.h"
class CScene;
class CGameObject;

class TankMonsterScript :
    public CScript
{
private:
    float           m_fTimer = 0.f;
    bool                     m_bLoopRepeat;  // [0] - > [1] - > [2] ---...[0]
    std::vector<class CatmullRomSpline> m_vecCatmullRom;

    bool m_bFinish = false;

    CGameObject* m_pTargetObj;


    float       m_fMoveSpeed= 10.f;
    XMFLOAT3    m_fMoveFrontDir;
    int         m_iRotateDir_ToPastAxis = 0;

    CScene* m_pCurScene;
    CGameObject* m_pGun;
    CGameObject* m_pTurret;
    CGameObject* m_pBody;


   int  m_iBulletMax = 20;
   std::vector<CGameObject*>   m_pBullets;
   XMFLOAT3                    m_xmf3_PastTargetRot;
   float m_fAcc = 0.f;
   int  m_iMoveDir = 1;


public:
    TankMonsterScript();
    TankMonsterScript(CGameObject* pOwner);
    virtual ~TankMonsterScript() override;

    virtual void Init() override;
    virtual void Update(float _fTimeElapsed) override;


    std::vector<CGameObject*> GetBullets() { return m_pBullets; }


    void Start();
    void RotateToPlayer(XMFLOAT3 vEndDir, XMFLOAT3 vStartDir,float _fTimeElapsed );
    void UpdateMove(float _fTimeElapsed);
    float GetRadAngle(XMFLOAT3 _Standard, XMFLOAT3 _Start);

    void setTarget(CGameObject* pObj) { m_pTargetObj = pObj; }
    void UpdatePosition(float _fTimeElapsed);
    void ResetCatmullRom();
    void ClearCatmullRom();
    XMFLOAT3 CalCulateCatmullRom(XMFLOAT3 _p0, XMFLOAT3 _p1, XMFLOAT3 _p2, XMFLOAT3 _p3, float _t);

    // Pos0 : 보간 시작 지점 / Pos1 : 보간 끝 지점 
    void CreateCatmullRomRoute(XMFLOAT3 _vPos0, std::vector<XMFLOAT3> _vecMainPos, XMFLOAT3 _vPos1);
    void CreateStartMap02Route(XMFLOAT3 _vStartPos);


    void Fire();


};

