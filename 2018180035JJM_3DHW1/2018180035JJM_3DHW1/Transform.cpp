#include "Transform.h"
#include "stdafx.h"

CTransform::CTransform(): CComponent()
{

	m_xmf3Position = XMFLOAT3(0.f, 0.f, 0.f);
	CComponent::SetComponentType(COMPONENT_TYPE::TRANSFORM);
}

CTransform::~CTransform()
{
}

void CTransform::ApplyRotationByRadian(XMFLOAT3 fRot)
{
	XMMATRIX matRotX = XMMatrixRotationX(m_xmf3Radian.x);
	XMMATRIX matRotY = XMMatrixRotationY(m_xmf3Radian.y);
	XMMATRIX matRotZ = XMMatrixRotationZ(m_xmf3Radian.z);
	m_matRot         = matRotX * matRotY * matRotZ;
	m_InverseRotation = DirectX::XMMatrixInverse(nullptr, m_matRot);


	//XMFLOAT4X4 matRot{};
	XMStoreFloat4x4(&m_xmf4x4Rotation, m_matRot);

	//m_xmf4x4Transform = Matrix4x4::Multiply(m_xmf4x4Transform, matRot);
}

void CTransform::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(fPitch, fYaw, fRoll);
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);
}

void CTransform::Rotate(XMFLOAT3& xmf3RotationAxis, float fAngle)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationAxis(xmf3RotationAxis, fAngle);
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

}

void CTransform::LookTo(XMFLOAT3& xmf3LookTo, XMFLOAT3& xmf3Up)
{

	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookToLH(m_xmf3Position, xmf3LookTo, xmf3Up);
	m_xmf4x4Transform._11 = xmf4x4View._11; m_xmf4x4Transform._12 = xmf4x4View._21; m_xmf4x4Transform._13 = xmf4x4View._31;
	m_xmf4x4Transform._21 = xmf4x4View._12; m_xmf4x4Transform._22 = xmf4x4View._22; m_xmf4x4Transform._23 = xmf4x4View._32;
	m_xmf4x4Transform._31 = xmf4x4View._13; m_xmf4x4Transform._32 = xmf4x4View._23; m_xmf4x4Transform._33 = xmf4x4View._33;

}

void CTransform::LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, xmf3Up);
	m_xmf4x4Transform._11 = xmf4x4View._11; m_xmf4x4Transform._12 = xmf4x4View._21; m_xmf4x4Transform._13 = xmf4x4View._31;
	m_xmf4x4Transform._21 = xmf4x4View._12; m_xmf4x4Transform._22 = xmf4x4View._22; m_xmf4x4Transform._23 = xmf4x4View._32;
	m_xmf4x4Transform._31 = xmf4x4View._13; m_xmf4x4Transform._32 = xmf4x4View._23; m_xmf4x4Transform._33 = xmf4x4View._33;

}

float CTransform::ToRadian(float fDegree)
{
	float fdeg = fDegree;
	float fRadian = DirectX::XMConvertToRadians(fdeg);
	return fRadian;
}

float CTransform::ToDegree(float fRadian)
{
	float fRad = fRadian;
	float fDegree = DirectX::XMConvertToDegrees(fRad);
	return fDegree;

}

XMFLOAT3 CTransform::ToRadian(XMFLOAT3 degree)
{
	XMFLOAT3 Deg2Rad = XMFLOAT3();
	Deg2Rad.x = ToRadian(degree.x);
	Deg2Rad.y = ToRadian(degree.y);
	Deg2Rad.z = ToRadian(degree.z);
	return Deg2Rad;
}

XMFLOAT3 CTransform::ToDegree(XMFLOAT3 Radian)
{
	XMFLOAT3 Rad2Deg = XMFLOAT3();
	Rad2Deg.x = ToDegree(Radian.x);
	Rad2Deg.y = ToDegree(Radian.y);
	Rad2Deg.z = ToDegree(Radian.z);
	return Rad2Deg;
}

XMFLOAT3 CTransform::GetVectorReverse(XMFLOAT3 xmf3Dir)
{
	XMFLOAT3 ReverseDir = XMFLOAT3(xmf3Dir.x * -1, xmf3Dir.y * -1, xmf3Dir.z * -1);
	ReverseDir = Vector3::Normalize(ReverseDir);
	return ReverseDir;
}



void CTransform::ComputeWorldTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	m_xmf4x4Transform = Matrix4x4::Multiply(m_xmf4x4Transform, m_xmf4x4Rotation);
	m_xmf4x4Transform = Matrix4x4::Multiply(m_xmf4x4Transform, m_xmf4x4Translation);
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4Transform, *pxmf4x4Parent) : m_xmf4x4Transform;
	
}

void CTransform::Init()
{

	m_xmf3Position = XMFLOAT3(0.f, 0.f, 0.f);

}

void CTransform::Update(float _fTimeElapsed)
{
	CComponent::Update(_fTimeElapsed);


	
}

void CTransform::FinalUpdate(float _fTimeElapsed)
{

	CGameObject* pObj = CComponent::GetOwner();
	std::string name = pObj->GetName();

// ROTATE
	m_xmf3Radian = ToRadian(m_xmf3Degree);
	UpdateRotation();

// TRANSFORM
	UpdateTransform();



// COMPUTE TRANSFORM

	// PARENT - O
	if (m_bTakeParent == true)
	{
		// TAKE PARENT WORLD TRANSFORM
		CGameObject* pParent = (CGameObject*)CComponent::GetOwner()->GetParent();
		if (pParent) {
			// 부모의 회전행렬만 뺀다. 
			if (m_bIgnoreParent_RotMat == true)
			{
				CTransform* pParent_Transform    = (CTransform*)pParent->GetComponent(COMPONENT_TYPE::TRANSFORM);
				XMFLOAT4X4	ParentWorldMat       = pParent_Transform->GetWorldMatrix();
				XMMATRIX	RotInv               = pParent_Transform->GetRotationMatInv();
				ParentWorldMat					 = Matrix4x4::Multiply(RotInv, ParentWorldMat);
				ComputeWorldTransform(&ParentWorldMat);
			}
			else
			{
				CTransform* pParent_Transform = (CTransform*)pParent->GetComponent(COMPONENT_TYPE::TRANSFORM);
				XMFLOAT4X4 ParentWorldMat = pParent_Transform->GetWorldMatrix();
				ComputeWorldTransform(&ParentWorldMat);
			}

		}
		else
			ComputeWorldTransform();
	}
	// PARENT - X
	else
		ComputeWorldTransform();
}

void CTransform::UpdateTransform()
{
	
	// ON Update Transform
	m_xmf4x4Transform._11 = m_tAxis.xmf3Right.x;
	m_xmf4x4Transform._12 = m_tAxis.xmf3Right.y;
	m_xmf4x4Transform._13 = m_tAxis.xmf3Right.z;

	m_xmf4x4Transform._21 = m_tAxis.xmf3Up.x;
	m_xmf4x4Transform._22 = m_tAxis.xmf3Up.y;
	m_xmf4x4Transform._23 = m_tAxis.xmf3Up.z;

	m_xmf4x4Transform._31 = m_tAxis.xmf3Look.x;
	m_xmf4x4Transform._32 = m_tAxis.xmf3Look.y;
	m_xmf4x4Transform._33 = m_tAxis.xmf3Look.z;

	m_xmf4x4Transform._41 = 0.f;
	m_xmf4x4Transform._42 = 0.f;
	m_xmf4x4Transform._43 = 0.f;

	XMMATRIX matTranslation = XMMatrixTranslation(m_xmf3Position.x, m_xmf3Position.y, m_xmf3Position.z);
	XMStoreFloat4x4(&m_xmf4x4Translation, matTranslation);
}

void CTransform::UpdateRotation()
{
	ApplyRotationByRadian(m_xmf3Radian);
}

void CTransform::UpdateWorldDir()
{

	XMFLOAT3 Right = XMFLOAT3(1.f, 0.f, 0.f);
	XMFLOAT3 Up    = XMFLOAT3(0.f, 1.f, 0.f);
	XMFLOAT3 Front = XMFLOAT3(0.f, 0.f, 1.f);

	const XMFLOAT3 vAxis[(UINT)DIR_TYPE::END] = { Right, Up, Front };

	XMFLOAT3 fDeg2Rad = ToRadian(m_xmf3Degree);
	XMMATRIX matRotX  = XMMatrixRotationX(fDeg2Rad.x);
	XMMATRIX matRotY  = XMMatrixRotationY(fDeg2Rad.y);
	XMMATRIX matRotZ  = XMMatrixRotationZ(fDeg2Rad.z);
	m_matRot          = matRotX * matRotY * matRotZ;

	for (size_t i = 0; i < std::size(m_xmf3WorldDir); ++i)
	{
		DirectX::XMStoreFloat3(&m_xmf3WorldDir[i], (DirectX::XMVector3TransformNormal(XMLoadFloat3(&vAxis[i]), m_matRot)));
		m_xmf3WorldDir[i] = Vector3::Normalize(m_xmf3WorldDir[i]);

	}

}

XMFLOAT3 CTransform::GetWorldDir(DIR_TYPE _eDirType)
{
	UpdateWorldDir(); 
	m_xmf3WorldDir[(UINT)DIR_TYPE::BACKWARD] = GetVectorReverse(m_xmf3WorldDir[(UINT)DIR_TYPE::FRONT]);
	m_xmf3WorldDir[(UINT)DIR_TYPE::LEFT]     = GetVectorReverse(m_xmf3WorldDir[(UINT)DIR_TYPE::RIGHT]);
	return m_xmf3WorldDir[(UINT)_eDirType];
}


void CTransform::SetPosition(float x, float y, float z)
{

	m_xmf3Position.x = x;
	m_xmf3Position.y = y;
	m_xmf3Position.z = z;

}

void CTransform::SetRotation(float x_pitch, float y_yaw, float z_roll)
{
	m_xmf3Degree = XMFLOAT3(x_pitch, y_yaw, z_roll);
}
