#include "stdafx.h"
#include "CatmullRomSpline.h"


CatmullRomSpline::CatmullRomSpline()
{
}

CatmullRomSpline::~CatmullRomSpline()
{
}


void CatmullRomSpline::CreateRoute(XMFLOAT3 _p0, XMFLOAT3 _p1, XMFLOAT3 _p2, XMFLOAT3 _p3)
{

    SetP0(_p0);
    SetP1(_p1);
    SetP2(_p2);
    SetP3(_p3);

}

float CatmullRomSpline::GetAngleRadian(XMFLOAT3 _v1, XMFLOAT3 _v2)
{
    XMFLOAT3 vFrom = _v1;
    vFrom = Vector3::Normalize(vFrom);
    XMVECTOR _vFrom = XMLoadFloat3(&vFrom);

    XMFLOAT3 vTo = _v2;
    vTo = Vector3::Normalize(vTo);
    XMVECTOR _vTo = XMLoadFloat3(&vTo);

    XMVECTOR X = XMVector3Dot(_vFrom, _vTo);
    float fCos = XMVectorGetX(X);
    if (fCos >= 1.f)  fCos = 1.f;
    if (fCos <= -1.f) fCos = -1.f;

    float fRad = acosf(fCos);					// 사이각 ( Radian )
    float fAng = fRad * (180.f / XM_PI);		// 사이각 ( Angle )	}

    return fRad;
}


XMFLOAT3 CatmullRomSpline::CalculatePosByCatmullRom()
{
    XMFLOAT3 ResultPos = XMFLOAT3(0.f, 0.f, 0.f);
    XMFLOAT3 P0 = m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P0];	// P0 위치 - P0 --> P2 
    XMFLOAT3 P1 = m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P1];	// P1 위치 - Start ( 시작 위치 ) t = 0.f
    XMFLOAT3 P2 = m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P2];	// P2 위치 - End   ( 끝 위치   ) t = 1.f
    XMFLOAT3 P3 = m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P3];	// P3 위치 - P1 --> P3 
    float t = m_fInterPolation_Control_Coefficient;			// t  보간 제어 계수
    XMVECTOR result = XMVectorCatmullRom(XMLoadFloat3(&P0)
        , XMLoadFloat3(&P1)
        , XMLoadFloat3(& P2)
         , XMLoadFloat3(&P3)
        , t);
    XMStoreFloat3(&ResultPos, result);

    return ResultPos;
}

void CatmullRomSpline::Update(float _fTimeElapsed)
{
    if (!m_bStart)
        return;
    // End 지점 도착 
    if (m_fInterPolation_Control_Coefficient >= 1.f)
    {
        m_fInterPolation_Control_Coefficient = 1.f;
        m_bIsEnd = true;
    }
    // Start 지점 
    else if (m_fInterPolation_Control_Coefficient <= 0.f)
    {
        m_fInterPolation_Control_Coefficient = 0.f;
    }

    // P1 -------> P2
    // t : ( Start ) 0.f ------ 1.f ( End )
    m_fInterPolation_Control_Coefficient += _fTimeElapsed * m_fSpeed;

}

