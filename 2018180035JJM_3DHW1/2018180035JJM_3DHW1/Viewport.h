#pragma once
class CViewport
{
public:
	CViewport() { }
	virtual ~CViewport() { }

	int							m_nLeft   = 0;
	int							m_nTop    = 0;
	int							m_nWidth  = 0;
	int							m_nHeight = 0;

	void SetViewport(int nLeft, int nTop, int nWidth, int nHeight);


};