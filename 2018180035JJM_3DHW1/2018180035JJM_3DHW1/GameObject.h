#pragma once

#include "global.h"
#include "MyUtility.h"
class CComponent;
class CMesh;
#include "Script.h"

class CScript;

/*
* Pen Style

	PS_SOLID
	PS_DASH
	PS_DOT
	PS_DASHDOT
	PS_DASHDOTDOT

*/


class CGameObject
{
public:
	CGameObject();
	~CGameObject();

private:
	std::string					m_Name;
	CGameObject*				m_pParent;
	bool						m_bActive;

// CHILD INFO
	std::vector<CGameObject*>	m_vecChild;
// COMPONENT INFO
	CComponent*					m_arrCom[static_cast<UINT>(COMPONENT_TYPE::END)];

// SCRIPT INFO
	std::vector<CScript*>		m_vecScript;

// DRAW INFO
	CMesh*						m_pMesh;

	int							m_iPenStlye		 = PS_SOLID;
	float							m_iPenWidth		 = 0;
	DWORD						m_dwDefaultColor = (DWORD)COLOR::BLACK;
	DWORD						m_dwColor        = (DWORD)COLOR::BLUE;

	bool						m_bCollide		  = false;
	BoundingOrientedBox			m_xmOOBB          = BoundingOrientedBox();
	CGameObject*				m_pObjectCollided = NULL;

// Scene 에 저장안하고 직접 바로 그리는 오브젝트 ex ) Explosive Obj
	bool						m_bRegisterRender_Direct = false;
	CMesh*						m_pMesh_Register_Direct =nullptr;
	std::vector<XMFLOAT4X4>		m_xmf4World_Direct{};
	float m_fCollideSphere_Radius = 4.f;

public:
	void SetCollide(bool _B) { m_bCollide = _B; }
	bool IsCollide() { return m_bCollide; }
	void setCollideRadius_Sphere(float _f) { m_fCollideSphere_Radius = _f; }

public:
	void AllowRenderDirect() { m_bRegisterRender_Direct = true; }
	void DenyRenderDirect() { m_bRegisterRender_Direct = false; }
	void RegisterWorld_RenderDirect(XMFLOAT4X4 worldMat) { m_xmf4World_Direct.push_back(worldMat);}
	void Clear_WorldRenderDirect() { m_xmf4World_Direct.clear(); }
	void RegisterMesh_RenderDirect(CMesh* pMesh) { m_pMesh_Register_Direct = pMesh; }

	CGameObject* GetCollideObj() { return m_pObjectCollided; }
	void SetCollideObj(CGameObject* pObj) { m_pObjectCollided = pObj; }
	BoundingOrientedBox GetBoundingBox() { return m_xmOOBB; }

public:
	bool IsActive();
	void Activate();
	void Deactivate();

	bool IsRenderObj(); // Mesh 가 있으면 Render Object 로 지정 

	void SetName(std::string _sName) { m_Name = _sName; }
	std::string GetName() { return m_Name; }

public:
	void Init();
	void Update(float _fTimeElapsed);
	void FinalUpdate(float _fTimeElapsed);

	void Render(HDC FrameBuffer, float _fTimeElapsed);
	void Render(HDC hDCFrameBuffer, XMFLOAT4X4* pxmf4x4World, CMesh* pMesh);

public:
	void AddChild(CGameObject* pObj);
	void AddScript(CScript* pSCript);
	void SetParent(CGameObject* pParent);
	CGameObject* GetParent() { return m_pParent; }
	CGameObject* FindChild(std::string _ChildName);
	std::vector<CGameObject*> GetAllChildInfo() { return m_vecChild; }


	void Move(XMFLOAT3 vNewPos);
	void Move(XMFLOAT3 vDir, float fSpeed);

	void UpdateBoundingBox();
	bool MyCollideCheck(CGameObject* pObj);



public:
	void AddComponent(CComponent* pCom);
	CComponent* GetComponent(COMPONENT_TYPE eComType);
	CScript* GetScript(SCRIPT_TYPE _eType);


	void SetMesh(CMesh* pMesh);
	void SetColor(COLOR eColor);
	void SetPenStyle(int dPenStyle) { m_iPenStlye = dPenStyle; }
	void SetPenWidth(float iPenWidth) { m_iPenWidth = iPenWidth; }



};

