#include "Mesh.h"
#include "stdafx.h"
#include "GraphicsPipeline.h"


void Draw2DLine(HDC hDCFrameBuffer, XMFLOAT3& f3PreviousProject, XMFLOAT3& f3CurrentProject)
{
	XMFLOAT3 f3Previous = CGraphicsPipeline::ScreenTransform(f3PreviousProject);
	XMFLOAT3 f3Current  = CGraphicsPipeline::ScreenTransform(f3CurrentProject);
	
	
	::MoveToEx(hDCFrameBuffer, (long)f3Previous.x, (long)f3Previous.y, NULL);
	::LineTo(hDCFrameBuffer, (long)f3Current.x, (long)f3Current.y);
}


CMesh::CMesh(int nPolygons)
{
	m_nPolygons = nPolygons;
	m_ppPolygons = new CPolygon * [nPolygons];

}

CMesh::~CMesh()
{
}

void CMesh::SetPolygon(int nIndex, CPolygon* pPolygon)
{
	if ((0 <= nIndex) && (nIndex < m_nPolygons)) 
		m_ppPolygons[nIndex] = pPolygon;

}


void CMesh::Render(HDC hDCFrameBuffer)
{
	XMFLOAT3 f3InitialProject, f3PreviousProject;
	bool bPreviousInside = false, bInitialInside = false, bCurrentInside = false, bIntersectInside = false;

	for (int j = 0; j < m_nPolygons; j++)
	{
		int nVertices = m_ppPolygons[j]->m_nVertices;
		CVertex* pVertices = m_ppPolygons[j]->m_pVertices;

		f3PreviousProject = f3InitialProject = CGraphicsPipeline::Project(pVertices[0].m_xmf3Position);
		bPreviousInside = bInitialInside = (-1.0f <= f3InitialProject.x) && (f3InitialProject.x <= 1.0f) && (-1.0f <= f3InitialProject.y) && (f3InitialProject.y <= 1.0f);
		
		for (int i = 1; i < nVertices; i++)
		{
			XMFLOAT3 f3CurrentProject = CGraphicsPipeline::Project(pVertices[i].m_xmf3Position);
			bCurrentInside = (-1.0f <= f3CurrentProject.x) && (f3CurrentProject.x <= 1.0f) && (-1.0f <= f3CurrentProject.y) && (f3CurrentProject.y <= 1.0f);
			if (((0.0f <= f3CurrentProject.z) && (f3CurrentProject.z <= 1.0f)) && ((bCurrentInside || bPreviousInside))) 
				::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3CurrentProject);
			f3PreviousProject = f3CurrentProject;
			bPreviousInside = bCurrentInside;
		}
		
		if (((0.0f <= f3InitialProject.z) && (f3InitialProject.z <= 1.0f)) && ((bInitialInside || bPreviousInside))) 
			::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3InitialProject);
	}
}

BOOL CMesh::RayIntersectionByTriangle(XMVECTOR& xmRayOrigin, XMVECTOR& xmRayDirection, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float* pfNearHitDistance)
{
	float fHitDistance;
	BOOL bIntersected = TriangleTests::Intersects(xmRayOrigin, xmRayDirection, v0, v1, v2, fHitDistance);
	if (bIntersected && (fHitDistance < *pfNearHitDistance)) *pfNearHitDistance = fHitDistance;

	return(bIntersected);
}

int CMesh::CheckRayIntersection(XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection, float* pfNearHitDistance)
{
	int nIntersections = 0;
	bool bIntersected = m_xmOOBB.Intersects(xmvPickRayOrigin, xmvPickRayDirection, *pfNearHitDistance);
	if (bIntersected)
	{
		for (int i = 0; i < m_nPolygons; i++)
		{
			switch (m_ppPolygons[i]->m_nVertices)
			{
			case 3:
			{
				XMVECTOR v0 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[0].m_xmf3Position));
				XMVECTOR v1 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[1].m_xmf3Position));
				XMVECTOR v2 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[2].m_xmf3Position));
				BOOL bIntersected = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance);
				if (bIntersected) nIntersections++;
				break;
			}
			case 4:
			{
				XMVECTOR v0 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[0].m_xmf3Position));
				XMVECTOR v1 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[1].m_xmf3Position));
				XMVECTOR v2 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[2].m_xmf3Position));
				BOOL bIntersected = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance);
				if (bIntersected) nIntersections++;
				v0 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[0].m_xmf3Position));
				v1 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[2].m_xmf3Position));
				v2 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[3].m_xmf3Position));
				bIntersected = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance);
				if (bIntersected) nIntersections++;
				break;
			}
			}
		}
	}
	return(nIntersections);
}

CCubeMesh::CCubeMesh(float fWidth, float fHeight, float fDepth) : CMesh(6)
{
	m_nPolygons = 6;

	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;
	float fHalfDepth = fDepth * 0.5f;

	CPolygon* pFrontFace = new CPolygon(4);
	CVertex vtx = CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth);
	pFrontFace->SetVertex(0, vtx);
	vtx = CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth);
	pFrontFace->SetVertex(1, vtx);
	vtx = CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth);
	pFrontFace->SetVertex(2, vtx);
	vtx = CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth);
	pFrontFace->SetVertex(3, vtx);
	SetPolygon(0, pFrontFace);

	CPolygon* pTopFace = new CPolygon(4);
	vtx = CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth);
	pTopFace->SetVertex(0, vtx);
	vtx = CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth);
	pTopFace->SetVertex(1, vtx);
	vtx = CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth);
	pTopFace->SetVertex(2, vtx);
	vtx = CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth);
	pTopFace->SetVertex(3, vtx );
	SetPolygon(1, pTopFace);

	CPolygon* pBackFace = new CPolygon(4);
	vtx = CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth);
	pBackFace->SetVertex(0, vtx);
	vtx = CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth);
	pBackFace->SetVertex(1, vtx);
	vtx = CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth);
	pBackFace->SetVertex(2, vtx);
	vtx = CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth);
	pBackFace->SetVertex(3, vtx);
	SetPolygon(2, pBackFace);

	CPolygon* pBottomFace = new CPolygon(4);
	vtx = CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth);
	pBottomFace->SetVertex(0, vtx);
	vtx = CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth);
	pBottomFace->SetVertex(1, vtx);
	vtx = CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth);
	pBottomFace->SetVertex(2, vtx);
	vtx = CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth);
	pBottomFace->SetVertex(3, vtx);
	SetPolygon(3, pBottomFace);

	CPolygon* pLeftFace = new CPolygon(4);
	vtx = CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth);
	pLeftFace->SetVertex(0, vtx);
	vtx = CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth);
	pLeftFace->SetVertex(1, vtx);
	vtx = CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth);
	pLeftFace->SetVertex(2, vtx);
	vtx = CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth);
	pLeftFace->SetVertex(3, vtx);
	SetPolygon(4, pLeftFace);

	CPolygon* pRightFace = new CPolygon(4);
	vtx = CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth);
	pRightFace->SetVertex(0, vtx);
	vtx = CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth);
	pRightFace->SetVertex(1, vtx);
	vtx = CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth);
	pRightFace->SetVertex(2, vtx);
	vtx = CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth);
	pRightFace->SetVertex(3, vtx);
	SetPolygon(5, pRightFace);

	m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

}

CFloorMesh::CFloorMesh(float fWidth, float fDepth, int nSubRects) : CMesh(nSubRects* nSubRects)
{
	float fHalfWidth = fWidth * 0.5f;
	float fHalfDepth = fDepth * 0.5f;
	float fCellWidth = fWidth * (1.0f / nSubRects);
	float fCellDepth = fDepth * (1.0f / nSubRects);

	int k = 0;
	CPolygon* pFace;
	for (int i = 0; i < nSubRects; i++)
	{
		for (int j = 0; j < nSubRects; j++)
		{
			pFace = new CPolygon(4);
			CVertex vtx = CVertex(-fHalfWidth + (i * fCellWidth), 0.0f, -fHalfDepth + (j * fCellDepth));
			pFace->SetVertex(0, vtx);
			vtx = CVertex(-fHalfWidth + ((i + 1) * fCellWidth), 0.0f, -fHalfDepth + (j * fCellDepth));
			pFace->SetVertex(1, vtx);
			vtx = CVertex(-fHalfWidth + ((i + 1) * fCellWidth), 0.0f, -fHalfDepth + ((j + 1) * fCellDepth));
			pFace->SetVertex(2, vtx);
			vtx = CVertex(-fHalfWidth + (i * fCellWidth), 0.0f, -fHalfDepth + ((j + 1) * fCellDepth));
			pFace->SetVertex(3, vtx);
			SetPolygon(k++, pFace);
		}
	}

	m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, 0.0f, fHalfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

}

CAirplaneMesh::CAirplaneMesh(float fWidth, float fHeight, float fDepth) : CMesh(24)
{
	m_nPolygons = 24;

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	float x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1 - (fy - y3)) / x1) * x2 + (fy - y3);
	int i = 0;

	//Upper Plane
	CPolygon* pFace = new CPolygon(3);
	
	CVertex vtx = CVertex(0.0f, +(fy + y3), -fz); pFace->SetVertex(0, vtx);
	vtx = CVertex(CVertex(+x1, -y1, -fz)); pFace->SetVertex(1, vtx);
	vtx = CVertex(0.0f, 0.0f, -fz); pFace->SetVertex(2, vtx);
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	vtx = CVertex(0.0f, +(fy + y3), -fz); pFace->SetVertex(0, vtx);
	vtx = CVertex(0.0f, 0.0f, -fz); pFace->SetVertex(1, vtx);
	vtx = CVertex(-x1, -y1, -fz); pFace->SetVertex(2, vtx);
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	vtx = CVertex(+x2, +y2, -fz);pFace->SetVertex(0,vtx);
	vtx = CVertex(+fx, -y3, -fz);pFace->SetVertex(1,vtx);
	vtx = CVertex(+x1, -y1, -fz);pFace->SetVertex(2, vtx);
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	vtx = CVertex(-x2, +y2, -fz);pFace->SetVertex(0, vtx);
	vtx = CVertex(-x1, -y1, -fz);pFace->SetVertex(1, vtx);
	vtx = CVertex(-fx, -y3, -fz);pFace->SetVertex(2, vtx);
	SetPolygon(i++, pFace);

	//Lower Plane
	pFace = new CPolygon(3);
	vtx = CVertex(0.0f, +(fy + y3), +fz);pFace->SetVertex(0, vtx);
	vtx = CVertex(0.0f, 0.0f, +fz)		;pFace->SetVertex(1, vtx);
	vtx = CVertex(+x1, -y1, +fz)		;pFace->SetVertex(2, vtx);
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	vtx = CVertex(0.0f, +(fy + y3), +fz); pFace->SetVertex(0, vtx);
	vtx = CVertex(-x1, -y1, +fz); pFace->SetVertex(1, vtx);
	vtx = CVertex(0.0f, 0.0f, +fz);pFace->SetVertex(2, vtx);
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	vtx = CVertex(+x2, +y2, +fz);pFace->SetVertex(0, vtx);
	vtx = CVertex(+x1, -y1, +fz);pFace->SetVertex(1, vtx);
	vtx = CVertex(+fx, -y3, +fz);pFace->SetVertex(2, vtx);
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	vtx =  CVertex(-x2, +y2, +fz);pFace->SetVertex(0, vtx);
	vtx =  CVertex(-fx, -y3, +fz);pFace->SetVertex(1, vtx);
	vtx =  CVertex(-x1, -y1, +fz);pFace->SetVertex(2, vtx);
	SetPolygon(i++, pFace);

	//Right Plane
	pFace = new CPolygon(3);
	vtx = CVertex(0.0f, +(fy + y3), -fz);pFace->SetVertex(0, vtx);
	vtx = CVertex(0.0f, +(fy + y3), +fz);pFace->SetVertex(1, vtx);
	vtx = CVertex(+x2, +y2, -fz)		;pFace->SetVertex(2, vtx);
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	vtx =  CVertex(+x2, +y2, -fz)			;pFace->SetVertex(0, vtx);
	vtx =  CVertex(0.0f, +(fy + y3), +fz)	;pFace->SetVertex(1, vtx);
	vtx =  CVertex(+x2, +y2, +fz)			;pFace->SetVertex(2, vtx);
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	vtx =  CVertex(+x2, +y2, -fz);pFace->SetVertex(0, vtx);
	vtx =  CVertex(+x2, +y2, +fz);pFace->SetVertex(1, vtx);
	vtx =  CVertex(+fx, -y3, -fz);pFace->SetVertex(2, vtx);
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	vtx = CVertex(+fx, -y3, -fz);pFace->SetVertex(0, vtx);
	vtx = CVertex(+x2, +y2, +fz);pFace->SetVertex(1, vtx);
	vtx = CVertex(+fx, -y3, +fz);pFace->SetVertex(2, vtx);
	SetPolygon(i++, pFace);

	//Back/Right Plane
	pFace = new CPolygon(3);
	vtx =CVertex(+x1, -y1, -fz);pFace->SetVertex(0, vtx);
	vtx =CVertex(+fx, -y3, -fz);pFace->SetVertex(1, vtx);
	vtx =CVertex(+fx, -y3, +fz);pFace->SetVertex(2, vtx);
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	vtx = CVertex(+x1, -y1, -fz);pFace->SetVertex(0, vtx);
	vtx = CVertex(+fx, -y3, +fz);pFace->SetVertex(1, vtx);
	vtx = CVertex(+x1, -y1, +fz);pFace->SetVertex(2, vtx);
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	vtx =  CVertex(0.0f, 0.0f, -fz)	;pFace->SetVertex(0, vtx);
	vtx =  CVertex(+x1, -y1, -fz)	;pFace->SetVertex(1, vtx);
	vtx =  CVertex(+x1, -y1, +fz)	;pFace->SetVertex(2, vtx);
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	vtx = CVertex(0.0f, 0.0f, -fz);pFace->SetVertex(0, vtx);
	vtx = CVertex(+x1, -y1, +fz);pFace->SetVertex(1, vtx);
	vtx = CVertex(0.0f, 0.0f, +fz);pFace->SetVertex(2, vtx);
	SetPolygon(i++, pFace);

	//Left Plane
	pFace = new CPolygon(3);
	vtx = CVertex(0.0f, +(fy + y3), +fz);pFace->SetVertex(0, vtx);
	vtx = CVertex(0.0f, +(fy + y3), -fz);pFace->SetVertex(1, vtx);
	vtx =  CVertex(-x2, +y2, -fz)		; pFace->SetVertex(2, vtx);
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	vtx = CVertex(0.0f, +(fy + y3), +fz)	; pFace->SetVertex(0, vtx);
	vtx = CVertex(-x2, +y2, -fz)			; pFace->SetVertex(1, vtx);
	vtx = CVertex(-x2, +y2, +fz)			; pFace->SetVertex(2, vtx);
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	vtx = CVertex(-x2, +y2, +fz); pFace->SetVertex(0, vtx);
	vtx = CVertex(-x2, +y2, -fz); pFace->SetVertex(1, vtx);
	vtx = CVertex(-fx, -y3, -fz); pFace->SetVertex(2, vtx);
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	vtx =  CVertex(-x2, +y2, +fz); pFace->SetVertex(0, vtx);
	vtx =  CVertex(-fx, -y3, -fz); pFace->SetVertex(1, vtx);
	vtx =  CVertex(-fx, -y3, +fz); pFace->SetVertex(2, vtx);
	SetPolygon(i++, pFace);

	//Back/Left Plane
	pFace = new CPolygon(3);
	vtx = CVertex(0.0f, 0.0f, -fz)	; pFace->SetVertex(0, vtx);
	vtx = CVertex(0.0f, 0.0f, +fz)	; pFace->SetVertex(1, vtx);
	vtx = CVertex(-x1, -y1, +fz)	; pFace->SetVertex(2, vtx);
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	vtx = CVertex(0.0f, 0.0f, -fz); pFace->SetVertex(0, vtx);
	vtx = CVertex(-x1, -y1, +fz)	; pFace->SetVertex(1, vtx);
	vtx = CVertex(-x1, -y1, -fz)	; pFace->SetVertex(2, vtx);
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	vtx =  CVertex(-x1, -y1, -fz); pFace->SetVertex(0, vtx);
	vtx =  CVertex(-x1, -y1, +fz); pFace->SetVertex(1, vtx);
	vtx =  CVertex(-fx, -y3, +fz); pFace->SetVertex(2, vtx);
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	vtx = CVertex(-x1, -y1, -fz); pFace->SetVertex(0, vtx);
	vtx = CVertex(-fx, -y3, +fz); pFace->SetVertex(1, vtx);
	vtx = CVertex(-fx, -y3, -fz); pFace->SetVertex(2, vtx);
	SetPolygon(i++, pFace);

	//m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

}

CAxisMesh::CAxisMesh(float fWidth, float fHeight, float fDepth) : CMesh(3)
{
	m_nPolygons = 3;

}

void CAxisMesh::Render(HDC hDCFrameBuffer)
{
	XMFLOAT3 f3PreviousProject = CGraphicsPipeline::Project(m_ppPolygons[0]->m_pVertices[0].m_xmf3Position);
	XMFLOAT3 f3CurrentProject = CGraphicsPipeline::Project(m_ppPolygons[0]->m_pVertices[1].m_xmf3Position);
	HPEN hPen = ::CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
	HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
	::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3CurrentProject);
	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);

	f3PreviousProject = CGraphicsPipeline::Project(m_ppPolygons[1]->m_pVertices[0].m_xmf3Position);
	f3CurrentProject = CGraphicsPipeline::Project(m_ppPolygons[1]->m_pVertices[1].m_xmf3Position);
	hPen = ::CreatePen(PS_SOLID, 0, RGB(0, 0, 255));
	hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
	::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3CurrentProject);
	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);

	f3PreviousProject = CGraphicsPipeline::Project(m_ppPolygons[2]->m_pVertices[0].m_xmf3Position);
	f3CurrentProject = CGraphicsPipeline::Project(m_ppPolygons[2]->m_pVertices[1].m_xmf3Position);
	hPen = ::CreatePen(PS_SOLID, 0, RGB(0, 255, 0));
	hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
	::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3CurrentProject);
	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);

}

CGunMesh::CGunMesh(float fWidth, float fHeight, float fDepth) : CMesh(6)
{
	m_nPolygons = 6;

	float fHalfWidth  = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;
	float fHalfDepth  = fDepth * 1.f;

	CPolygon* pFrontFace = new CPolygon(4);
	CVertex vtx = CVertex(-fHalfWidth, +fHalfHeight, -0.f);
	pFrontFace->SetVertex(0, vtx);
	vtx = CVertex(+fHalfWidth, +fHalfHeight, -0.f);
	pFrontFace->SetVertex(1, vtx);
	vtx = CVertex(+fHalfWidth, -fHalfHeight, -0.f);
	pFrontFace->SetVertex(2, vtx);
	vtx = CVertex(-fHalfWidth, -fHalfHeight, -0.f);
	pFrontFace->SetVertex(3, vtx);
	SetPolygon(0, pFrontFace);

	CPolygon* pTopFace = new CPolygon(4);
	vtx = CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth);
	pTopFace->SetVertex(0, vtx);
	vtx = CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth);
	pTopFace->SetVertex(1, vtx);
	vtx = CVertex(+fHalfWidth, +fHalfHeight, 0.f);
	pTopFace->SetVertex(2, vtx);
	vtx = CVertex(-fHalfWidth, +fHalfHeight, 0.f);
	pTopFace->SetVertex(3, vtx);
	SetPolygon(1, pTopFace);

	CPolygon* pBackFace = new CPolygon(4);
	vtx = CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth);
	pBackFace->SetVertex(0, vtx);
	vtx = CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth);
	pBackFace->SetVertex(1, vtx);
	vtx = CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth);
	pBackFace->SetVertex(2, vtx);
	vtx = CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth);
	pBackFace->SetVertex(3, vtx);
	SetPolygon(2, pBackFace);

	CPolygon* pBottomFace = new CPolygon(4);
	vtx = CVertex(-fHalfWidth, -fHalfHeight, 0.f);
	pBottomFace->SetVertex(0, vtx);
	vtx = CVertex(+fHalfWidth, -fHalfHeight, 0.f);
	pBottomFace->SetVertex(1, vtx);
	vtx = CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth);
	pBottomFace->SetVertex(2, vtx);
	vtx = CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth);
	pBottomFace->SetVertex(3, vtx);
	SetPolygon(3, pBottomFace);

	CPolygon* pLeftFace = new CPolygon(4);
	vtx = CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth);
	pLeftFace->SetVertex(0, vtx);
	vtx = CVertex(-fHalfWidth, +fHalfHeight, 0.f);
	pLeftFace->SetVertex(1, vtx);
	vtx = CVertex(-fHalfWidth, -fHalfHeight, 0.f);
	pLeftFace->SetVertex(2, vtx);
	vtx = CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth);
	pLeftFace->SetVertex(3, vtx);
	SetPolygon(4, pLeftFace);

	CPolygon* pRightFace = new CPolygon(4);
	vtx = CVertex(+fHalfWidth, +fHalfHeight, 0.f);
	pRightFace->SetVertex(0, vtx);
	vtx = CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth);
	pRightFace->SetVertex(1, vtx);
	vtx = CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth);
	pRightFace->SetVertex(2, vtx);
	vtx = CVertex(+fHalfWidth, -fHalfHeight, 0.f);
	pRightFace->SetVertex(3, vtx);
	SetPolygon(5, pRightFace);

	m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

}
