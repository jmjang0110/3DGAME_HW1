#pragma once
#include "Vertex.h"

class CPolygon
{
public:
	CPolygon() { }
	CPolygon(int nVertices);
	~CPolygon();

	int					m_nVertices = 0;
	CVertex*			m_pVertices = nullptr;

	void SetVertex(int nIndex, CVertex& vertex);
};

