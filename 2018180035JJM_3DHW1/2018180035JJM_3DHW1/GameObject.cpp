#include "GameObject.h"
#include "stdafx.h"
#include "Component.h"
#include "Transform.h"
#include "GraphicsPipeline.h"

#include "Mesh.h"
#include "Transform.h"
#include "Script.h"





CGameObject::CGameObject()
{
    m_bActive = true;
    m_dwColor = (DWORD)RGB(255, 0, 0);
}

CGameObject::~CGameObject()
{
    for (int i = 0; i < (UINT)COMPONENT_TYPE::END; ++i) {
        if (m_arrCom[i] != nullptr) {
            delete m_arrCom[i];
            m_arrCom[i] = nullptr;
        }
    }

    for (auto pScript : m_vecScript) {
        delete pScript;
    }

    m_vecScript.clear();
    if(m_pMesh)
        m_pMesh->Release();

}

bool CGameObject::IsActive()
{
    return m_bActive;
}

void CGameObject::Activate()
{
    m_bActive = true;
    for (int i = 0; i < (UINT)COMPONENT_TYPE::END; ++i) {
        m_arrCom[i]->Activate();
    }

    for (const auto pScript : m_vecChild) {
        pScript->Activate();
    }


}

void CGameObject::Deactivate()
{
    m_bActive = false;

    for (int i = 0; i < (UINT)COMPONENT_TYPE::END; ++i) {
        m_arrCom[i]->Deactivate();
    }

    for (const auto pScript : m_vecChild) {
        pScript->Deactivate();
    }


}

bool CGameObject::IsRenderObj()
{
    if (m_pMesh)
        return true;
    else
        return false;

}

void CGameObject::Init()
{

}

void CGameObject::Update(float _fTimeElapsed)
{

// UPDATE COMPONENT
    for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i) {
        if (m_arrCom[i] != nullptr) {
            m_arrCom[i]->Update(_fTimeElapsed);
        }
    }

// UPDATE SCRIPT
    for (const auto pScript : m_vecScript) {
        if (pScript)
            pScript->Update(_fTimeElapsed);
    }

 // UPDATE CHILD
    for (const auto pChild : m_vecChild) {
        if (pChild->IsActive())
            pChild->Update(_fTimeElapsed);
    }


}

void CGameObject::FinalUpdate(float _fTimeElapsed)
{

// FINAL UPDATE COMPONENT
    for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i) {
        if (m_arrCom[i] != nullptr) {
            m_arrCom[i]->FinalUpdate(_fTimeElapsed);
        }
    }
// FINAL UPDATE CHILD
    for (const auto pChild : m_vecChild) {
        if (pChild->IsActive())
            pChild->FinalUpdate(_fTimeElapsed);
    }

    UpdateBoundingBox();

}

void CGameObject::Render(HDC hDCFrameBuffer, XMFLOAT4X4* pxmf4x4World, CMesh* pMesh)
{
    if (pMesh)
    {
        CGraphicsPipeline::SetWorldTransform(pxmf4x4World);

        HPEN hPen = ::CreatePen(PS_SOLID, 0, m_dwColor);
        HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);

        pMesh->Render(hDCFrameBuffer);

        ::SelectObject(hDCFrameBuffer, hOldPen);
        ::DeleteObject(hPen);
    }
}

void CGameObject::Render(HDC FrameBuffer, float _fTimeElapsed)
{

    if (!m_bRegisterRender_Direct) {
        if (m_pMesh) {

            // SETTING WORLD TRANSFORM
            CTransform* pTransform = (CTransform*)this->GetComponent(COMPONENT_TYPE::TRANSFORM);
            XMFLOAT4X4 pxmf4x4World = pTransform->GetWorldMatrix();

            CGraphicsPipeline::SetWorldTransform(&pxmf4x4World);

            HPEN hPen = ::CreatePen(m_iPenStlye, m_iPenWidth, m_dwColor);
            HPEN hOldPen = (HPEN)::SelectObject(FrameBuffer, hPen);

            // MESH RENDER 
            m_pMesh->Render(FrameBuffer);

            ::SelectObject(FrameBuffer, hOldPen);
            ::DeleteObject(hPen);

        }
    }
    else {
        for (int i = 0; i < m_xmf4World_Direct.size(); ++i) {
        Render(FrameBuffer, &m_xmf4World_Direct[i], m_pMesh_Register_Direct);
        }

    }



// RENDER CHILD 
    for (const auto pChild : m_vecChild) {
        if (pChild->IsActive())
            pChild->Render(FrameBuffer, _fTimeElapsed);
    }
    
}

void CGameObject::AddChild(CGameObject* pObj)
{
    pObj->SetParent(this);
    m_vecChild.push_back(pObj);

}

void CGameObject::AddScript(CScript* pSCript)
{
    pSCript->SetOWner(this);
    m_vecScript.push_back(pSCript);

}

void CGameObject::SetParent(CGameObject* pParent)
{
    m_pParent = pParent;
}

CGameObject* CGameObject::FindChild(std::string _ChildName)
{
    CGameObject* pReturnObj = nullptr;
    for (const auto pChild : m_vecChild) {
        if (pChild->GetName() == _ChildName) 
        {
            pReturnObj = pChild;
            break;
        }
        else
        {
            pReturnObj = pChild->FindChild(_ChildName);
            if (pReturnObj != nullptr)
                break;
        }
    }
    return pReturnObj;
}

void CGameObject::Move(XMFLOAT3 vNewPos)
{

    CTransform* pTransform = (CTransform*)this->GetComponent(COMPONENT_TYPE::TRANSFORM);
    pTransform->SetPosition(vNewPos);

}

void CGameObject::Move(XMFLOAT3 vDir, float fSpeed)
{
    CTransform* pTransform = (CTransform*)this->GetComponent(COMPONENT_TYPE::TRANSFORM);
    XMFLOAT3 vCurPos = pTransform->GetCurPosition();

    XMFLOAT3 vNormalDir = Vector3::Normalize(vDir);
    vCurPos.x += vNormalDir.x * fSpeed;
    vCurPos.y += vNormalDir.y * fSpeed;
    vCurPos.z += vNormalDir.z * fSpeed;

    pTransform->SetPosition(vCurPos);

}

void CGameObject::UpdateBoundingBox()
{
    if (m_pMesh)
    {
        CTransform* pTrans = (CTransform*)this->GetComponent(COMPONENT_TYPE::TRANSFORM);
        XMFLOAT4X4 pWorld = pTrans->GetWorldMatrix();
        m_pMesh->m_xmOOBB.Transform(m_xmOOBB, XMLoadFloat4x4(&pWorld));
        XMStoreFloat4(&m_xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
    }

    for (auto pChild : m_vecChild) {
        pChild->UpdateBoundingBox();
    }
}

bool CGameObject::MyCollideCheck(CGameObject* pObj)
{
    CTransform* pOtherTrans = (CTransform*)pObj->GetComponent(COMPONENT_TYPE::TRANSFORM);
    XMFLOAT3 vOtherPos = pOtherTrans->GetCurPosition();

    CTransform* pMyTrans = (CTransform*)this->GetComponent(COMPONENT_TYPE::TRANSFORM);
    XMFLOAT3 vMyPos = pMyTrans->GetCurPosition();
    
    XMFLOAT3 vDist = Vector3::Subtract(vOtherPos, vMyPos);
    float fDist = Vector3::Length(vDist);

    if (fDist <= m_fCollideSphere_Radius)
        return true;
    else
        return false;

}

void CGameObject::AddComponent(CComponent* pCom)
{
    pCom->SetOwner(this);
    COMPONENT_TYPE comType = pCom->GetComponentType();
    m_arrCom[(UINT)comType] = pCom;

}

CComponent* CGameObject::GetComponent(COMPONENT_TYPE eComType)
{
    return m_arrCom[(UINT)eComType];

}

CScript* CGameObject::GetScript(SCRIPT_TYPE _eType)
{
    for (auto pScript : m_vecScript) {
        if (pScript->GetType() == _eType)
            return pScript;

    }
    return nullptr;

}

void CGameObject::SetMesh(CMesh* pMesh)
{
    m_pMesh = pMesh;


}

void CGameObject::SetColor(COLOR eColor)
{
    m_dwColor = (DWORD)eColor;
}
