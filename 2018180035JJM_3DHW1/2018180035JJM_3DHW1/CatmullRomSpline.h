#pragma once

enum class CATMULL_ROM_SPLINE
{
    P0,
    P1,
    P2,
    P3,
    END,

};
/*

*                       →→→
*                   ↗       ↘
*       t = 0.f                      t = 1.f
         (start)  [P1]          [P2] ( End )
                     ↘      ↗
                        ↘ ↗
                        ↗ ↘
                      ↗     ↘
                    ↗         ↘
                  ↗             ↘
             [P0]                  [P3]
     t = 0.f                           t = 1.f
*/


class CatmullRomSpline
{
public:
    XMFLOAT3  m_SplinePos[(UINT)CATMULL_ROM_SPLINE::END];       // 정점 위치 저장 P0, P1, P2, P3
    float m_fInterPolation_Control_Coefficient = 0.f;       // 보간 제어 계수 
    float m_fSpeed = 0.1f;
    bool  m_bIsEnd = false;
    bool  m_bStart = false;


public:
    // [ SET ]
    void SetP0(XMFLOAT3 _p0) { m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P0] = _p0; }
    void SetP1(XMFLOAT3 _p1) { m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P1] = _p1; }
    void SetP2(XMFLOAT3 _p2) { m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P2] = _p2; }
    void SetP3(XMFLOAT3 _p3) { m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P3] = _p3; }
    void SetStartPos(XMFLOAT3 _vStart) { m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P1] = _vStart; }
    void SetEndPos(XMFLOAT3 _vEnd) { m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P2] = _vEnd; }
    void SetSpeed(float _fSpeed) { m_fSpeed = _fSpeed; }

    bool IsEnd() { return m_bIsEnd; }
    bool IsStart() { return m_bStart; }

    // [ GET ]
    XMFLOAT3 GetP0() { return m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P0]; }
    XMFLOAT3 GetP1() { return m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P1]; }
    XMFLOAT3 GetP2() { return m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P2]; }
    XMFLOAT3 GetP3() { return m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P3]; }

    // [ CREATE ]
    // 경로 지정 생성 
    void CreateRoute(XMFLOAT3 _p0, XMFLOAT3 _p1, XMFLOAT3 _p2, XMFLOAT3 _p3);



    // 캣멀롬 스플라인 결과값 계산  
    XMFLOAT3 CalculatePosByCatmullRom();
    // 보간 제어 계수값 업데이트 
    void Update(float _fTimeElapsed);

    float GetAngleRadian(XMFLOAT3 _v1, XMFLOAT3 _v2);


public:
    CatmullRomSpline();
    ~CatmullRomSpline();
};

