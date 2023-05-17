#pragma once

#include "Viewport.h"
#include "GameObject.h"
#include "Component.h"


class CCamera : public CComponent
{
private:

	float						m_fFOVAngle						= 90.0f;
	float						m_fProjectRectDistance			= 1.0f;
	float						m_fAspectRatio					= float(FRAMEBUFFER_WIDTH) / float(FRAMEBUFFER_HEIGHT);

// MATRIX 
	XMFLOAT4X4					m_xmf4x4View					= Matrix4x4::Identity();	/// VIEW 
	XMFLOAT4X4					m_xmf4x4InverseView             = Matrix4x4::Identity();	/// INVERSE VIEW 

	XMFLOAT4X4					m_xmf4x4PerspectiveProject		= Matrix4x4::Identity();	/// PERSPECTIVE 
	XMFLOAT4X4					m_xmf4x4ViewPerspectiveProject	= Matrix4x4::Identity();	/// VIEW * PERSPECTIVE 

	XMFLOAT4X4					m_xmf4x4OrthographicProject     = Matrix4x4::Identity();	/// ORTHOGRAPHIC
	XMFLOAT4X4					m_xmf4x4ViewOrthographicProject = Matrix4x4::Identity();	/// VIEW * ORTHOGRAPHIC

// VIEWPORT
	CViewport					m_Viewport;

private:
// FRUSTUM 
	BoundingFrustum				m_xmFrustumView  = BoundingFrustum();
	BoundingFrustum				m_xmFrustumWorld = BoundingFrustum();

// OFFSET POS 
	CGameObject*				m_pTargetObject;
	XMFLOAT3					m_xmf3OffsetFromTargetObj;

public:
	void SetCameraOffset(XMFLOAT3& xmf3CameraOffset);
	void SetTargetObject(CGameObject* pObj) { m_pTargetObject = pObj; }

public:
	void SetFOVAngle(float fFOVAngle);

	void GenerateViewMatrix();
	void GeneratePerspectiveProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fFOVAngle);
	void GenerateOrthographicProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fWidth, float hHeight);
	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight);

	void SetLookAt(XMFLOAT3& EyePos, XMFLOAT3& LookatPos, XMFLOAT3& xmf3Up);

	void Move(XMFLOAT3& xmf3Shift);
	void Move(float x, float y, float z);
	void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
	void Update(CGameObject* pTargetObj, XMFLOAT3& xmf3LookAt, float fTimeElapsed = 0.016f);
	void Update(float _fTimeElapsed = 0.016f) override;
	void FinalUpdate(float _fTimeElapsed) override;



	bool IsInFrustum(BoundingOrientedBox& xmBoundingBox);


public:
	XMFLOAT4X4 GetPerspectiveProjectMat() { return m_xmf4x4PerspectiveProject; }
	XMFLOAT4X4 GetViewPerspectiveProjectMat() { return m_xmf4x4ViewPerspectiveProject; }
	CViewport& GetViewport() { return m_Viewport; }

public:
	CCamera();
	virtual ~CCamera();

};

