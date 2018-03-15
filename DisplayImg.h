#pragma once
#include "afxwin.h"
class CDisplayImg :
	public CStatic
{
public:
	CDisplayImg();
	~CDisplayImg();

	void DisplayImage(const Mat& imgData);

private:

	Mat m_inMat;
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
};

