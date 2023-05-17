#pragma once

#include "Component.h"

enum class DIR_TYPE
{
	RIGHT,
	UP,
	FRONT,

	BACKWARD,
	LEFT,
	END,
};
struct AXIS {
	XMFLOAT3	xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3	xmf3Up    = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3	xmf3Look  = XMFLOAT3(0.0f, 0.0f, 1.0f);

public:
	AXIS() {}
	AXIS(XMFLOAT3 right, XMFLOAT3 up, XMFLOAT3 look) {
		xmf3Right = right;
		xmf3Up = up;
		xmf3Look = look;
	}
	~AXIS() {}
};

class CTransform :
	public CComponent
{
private:
// AXIS INFO
	AXIS						m_tAxis;
	
	XMFLOAT3					m_xmf3WorldDir[(UINT)DIR_TYPE::END]; 
	XMMATRIX					m_matRot;

// POSITION INFO
	XMFLOAT3					m_xmf3Position		= XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Offset        = XMFLOAT3(0.f, 0.f, 0.f); // 부모로 부터 오프셋 값 
// ROTATION INFO
	XMFLOAT3					m_xmf3Degree       = XMFLOAT3(0.f, 0.f, 0.f);
	XMFLOAT3					m_xmf3Radian       = XMFLOAT3(0.f, 0.f, 0.f);
	XMFLOAT3					m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	float						m_fRotationSpeed   = 0.0f;

	XMFLOAT3					m_xmf3Scale = XMFLOAT3(0.f, 0.f, 0.f);

// MATRIX
	XMFLOAT4X4					m_xmf4x4Transform = Matrix4x4::Identity();
	XMFLOAT4X4					m_xmf4x4World     = Matrix4x4::Identity();

	XMFLOAT4X4					m_xmf4x4Translation = Matrix4x4::Identity();
	XMFLOAT4X4					m_xmf4x4Rotation    = Matrix4x4::Identity();

	XMMATRIX					m_InverseRotation{};
	bool						m_bIgnoreParent_RotMat = false;


	bool						m_bTakeParent = TRUE; // 부모의 영향을 받는지

public:
	CTransform();
	virtual ~CTransform();



public:
	void		UpdateWorldDir();
	XMFLOAT3	GetWorldDir(DIR_TYPE _eDirType);
	XMMATRIX    GetRotationMatInv() { return m_InverseRotation; }
	void IgnoreParentRotation(bool _B) { m_bIgnoreParent_RotMat = _B; }

public:
// GET
	AXIS GetAxis()					{ return m_tAxis; }
	float GetPitch()				{ return m_xmf3Degree.x; }
	float GetYaw()					{ return m_xmf3Degree.y; }
	float GetRoll()					{ return m_xmf3Degree.z; }
	XMFLOAT4X4 GetTransformMatrix()	{ return m_xmf4x4Transform; }
	XMFLOAT4X4 GetWorldMatrix()		{ return m_xmf4x4World; }
	XMFLOAT3 GetCurPosition()		{ return m_xmf3Position;  }
	XMFLOAT3 GetRotation()			{ return m_xmf3Degree; }
	XMFLOAT3 GetScale() { return m_xmf3Scale; }

	
// SET
	void SetScale(XMFLOAT3 scale) { m_xmf3Scale = scale; }
	void SetAxis(AXIS tAxis)	{ m_tAxis = tAxis; }
	void SetPitch(float fPitch) { m_xmf3Degree.x = fPitch; }
	void SetYaw(float fYaw)		{ m_xmf3Degree.y = fYaw; }
	void SetRoll(float fRoll)	{ m_xmf3Degree.z = fRoll; }

	void SetRotationAxis(XMFLOAT3& xmf3RotationAxis) { m_xmf3RotationAxis = Vector3::Normalize(xmf3RotationAxis); }
	void SetRotationSpeed(float fSpeed) { m_fRotationSpeed = fSpeed; }


	void ApplyRotationByRadian(XMFLOAT3 fRot);
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3& xmf3Axis, float fAngle);

	void LookTo(XMFLOAT3& xmf3LookTo, XMFLOAT3& xmf3Up);
	void LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);

	float ToRadian(float fDegree);
	float ToDegree(float fRadian);

	XMFLOAT3 ToRadian(XMFLOAT3 degree);
	XMFLOAT3 ToDegree(XMFLOAT3 Radian);

	XMFLOAT3 GetVectorReverse(XMFLOAT3 xmf3Dir);

public:
	void ComputeWorldTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	void SetAffectParentTransform(bool _b) { m_bTakeParent = _b; }

public:

	void Init() override;
	void Update(float _fTimeElapsed) override;
	void FinalUpdate(float _fTimeElapsed) override;

public:
	void UpdateTransform();
	void UpdateRotation();
	

public:
	void SetPosition(XMFLOAT3 pos) { m_xmf3Position = pos; }
	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetRotation(XMFLOAT3 Rot) { m_xmf3Degree = Rot; }

	void SetOffset(XMFLOAT3 offset) { m_xmf3Offset = offset; }
	XMFLOAT3	GetOffset() { return m_xmf3Offset; }

};

