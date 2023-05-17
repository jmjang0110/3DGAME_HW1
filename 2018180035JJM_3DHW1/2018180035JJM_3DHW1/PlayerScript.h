#pragma once
#include "Script.h"
#include "MyUtility.h"

//#include "Key.h"

class CGameObject;
class CScene;


class PlayerScript :
    public CScript
{
private:
    CScene* m_pCurScene;
    CGameObject* m_pGun;
    CGameObject* m_pTurret;



// MOVE KEY
    KEY         m_eFrontKey     = KEY::W;
    KEY         m_eLeftKey      = KEY::A;
    KEY         m_eBackKey      = KEY::S;
    KEY         m_eRightKey     = KEY::D;

// GUN MOVE KEY 
    KEY         m_eTurnLeftKey  = KEY::Q;
    KEY         m_eTurnRightKey = KEY::E;
    KEY         m_eTurnUpKey    = KEY::R;
    KEY         m_eTurnDownKey  = KEY::F;

// ATTACK KEY
    KEY         m_eJumpKey      = KEY::SPACE;
    KEY         m_eAttackKey    = KEY::LBTN;

private:
    std::vector<CGameObject*>   m_pBullets;
    UINT                        m_iBulletMax         = 50; 
    float                       m_fGunRotation_Speed = 1.5;

// CAMERA
    CGameObject*                m_pMainCam;
    XMFLOAT3					m_xmf3OffsetFromTargetObj;
    float                       m_fMaxOffsetDistance = 20.f;
    float                       m_fOffsetHeight      = 10.f;
    float                       m_fRotSpeed          = 2.f;
    float                       m_fMoveAccRot        = 0.f;

    int                         m_iRotateDir_Past_ForCamAxis = 0;

    XMFLOAT3                    m_xmf3_PastCamRot;

public:
    std::vector<CGameObject*> GetBullets() { return m_pBullets; }


public:
    PlayerScript();
    PlayerScript(CGameObject* pOwner);
    virtual ~PlayerScript() override;

    virtual void Init() override;
    virtual void Update(float _fTimeElapsed) override;

public:
    void UpdateCamera_ThirdPersonMode(float _fTimeElapsed);
    void Rotate_PlayerAxis_Sync_Camera_Axis(float _fTimeElapsed); // 카메라 Front 벡터쪽으로 Player 회전  
    void RotatePlayer(XMFLOAT3 vEndDir, XMFLOAT3 vStartDir, float _fTimeElapsed); // FinalDir 방향으로 Player 가 앞을 바라보게 회전

    float GetRadAngle(XMFLOAT3 _Standard, XMFLOAT3 _Start);
    bool CheckRotateFinish(float _fRot_DT);


public:

    void Update_InputKey(float _fTimeElapsed);

    

};

