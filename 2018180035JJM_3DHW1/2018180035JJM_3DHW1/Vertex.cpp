#include "stdafx.h"
#include "Vertex.h"

CVertex::CVertex()
{
	m_xmf3Position = XMFLOAT3{ 0.f, 0.f ,0.f };
}

CVertex::CVertex(float x, float y, float z)
{
	m_xmf3Position = XMFLOAT3(x, y, z);
}

CVertex::~CVertex()
{
}
