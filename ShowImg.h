#if !defined(AFX_SHOWIMAGE_H__790CEF95_07E1_489B_A3E3_B99DD0367837__INCLUDED_)
#define AFX_SHOWIMAGE_H__790CEF95_07E1_489B_A3E3_B99DD0367837__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShowImage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShowImage window

class CShowImage : public CStatic
{
// Construction
public:
	CShowImage();
	void DisplayImage(const Mat& imgData);

// Attributes
private:

	Mat m_inMat;
	Point m_starPt;
	Rect m_showRect;

	bool m_bLdown;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShowImage)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CShowImage();

	// Generated message map functions
protected:
	//{{AFX_MSG(CShowImage)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHOWIMAGE_H__790CEF95_07E1_489B_A3E3_B99DD0367837__INCLUDED_)
