#pragma once

#include "stdafx.h"

#include "Polygon.h"
#include "Vertex.h"

class CMesh
{
public:
	CMesh() { }
	CMesh(int nPolygons);
	virtual ~CMesh();

private:
	int							m_nReferences	= 1;

public:
	void AddRef() { m_nReferences++; }
	void Release() { m_nReferences--; if (m_nReferences <= 0) delete this; }

protected:
	int							m_nPolygons		= 0;
	CPolygon**					m_ppPolygons	= nullptr;

public:
	BoundingOrientedBox			m_xmOOBB = BoundingOrientedBox();

public:
	void SetPolygon(int nIndex, CPolygon* pPolygon);
	virtual void Render(HDC hDCFrameBuffer);

	BOOL RayIntersectionByTriangle(XMVECTOR& xmRayOrigin, XMVECTOR& xmRayDirection, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float* pfNearHitDistance);
	int CheckRayIntersection(XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection, float* pfNearHitDistance);


};

class CCubeMesh : public CMesh
{
public:
	CCubeMesh(float fWidth = 4.0f, float fHeight = 4.0f, float fDepth = 4.0f);
	virtual ~CCubeMesh() { }
};

class CFloorMesh : public CMesh
{
public:
	CFloorMesh(float fWidth = 4.0f, float fDepth = 4.0f, int nSubRects = 20);
	virtual ~CFloorMesh() { }
};

class CAirplaneMesh : public CMesh
{
public:
	CAirplaneMesh(float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 4.0f);
	virtual ~CAirplaneMesh() { }
};

class CAxisMesh : public CMesh
{
public:
	CAxisMesh(float fWidth = 4.0f, float fHeight = 4.0f, float fDepth = 4.0f);
	virtual ~CAxisMesh() { }

	virtual void Render(HDC hDCFrameBuffer);
};

class CGunMesh : public CMesh
{
public:
	CGunMesh(float fWidth = 4.0f, float fHeight = 4.0f, float fDepth = 4.0f);
	virtual ~CGunMesh() { }
};