// ShowImage.cpp : implementation file
//

#include "stdafx.h"
#include "ShowImg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShowImage

CShowImage::CShowImage()
{
	m_showRect = Rect(0, 0, 0, 0);
	m_bLdown = false;
}

CShowImage::~CShowImage()
{	

}


BEGIN_MESSAGE_MAP(CShowImage, CStatic)
	//{{AFX_MSG_MAP(CShowImage)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShowImage message handlers

void CShowImage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	if (m_inMat.empty())
		return;

	Mat imgTmp;
	CRect rect;
	GetClientRect(&rect);  // 获取控件大小
	resize(m_inMat, imgTmp, Size(rect.Width(), rect.Height()));// 缩放Mat并备份

	rectangle(imgTmp, m_showRect, Scalar(0, 0, 255), 2);

	// 转一下格式,这段可以放外面,
	switch (imgTmp.channels())
	{
	case 1:
		cvtColor(imgTmp, imgTmp, CV_GRAY2BGRA); // GRAY单通道
		break;
	case 3:
		cvtColor(imgTmp, imgTmp, CV_BGR2BGRA);  // BGR三通道
		break;
	default:
		break;
	}

	int pixelBytes = imgTmp.channels()*(imgTmp.depth() + 1); // 计算一个像素多少个字节
	// 制作bitmapinfo(数据头)
	BITMAPINFO bitInfo;
	bitInfo.bmiHeader.biBitCount = 8 * pixelBytes;
	bitInfo.bmiHeader.biWidth = imgTmp.cols;
	bitInfo.bmiHeader.biHeight = -imgTmp.rows;
	bitInfo.bmiHeader.biPlanes = 1;
	bitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitInfo.bmiHeader.biCompression = BI_RGB;
	bitInfo.bmiHeader.biClrImportant = 0;
	bitInfo.bmiHeader.biClrUsed = 0;
	bitInfo.bmiHeader.biSizeImage = 0;
	bitInfo.bmiHeader.biXPelsPerMeter = 0;
	bitInfo.bmiHeader.biYPelsPerMeter = 0;

	// Mat.data+ bitmap数据头 -> MFC
	CDC *pDC = GetDC();
	::StretchDIBits(
		pDC->GetSafeHdc(),
		0, 0, rect.Width(), rect.Height(),
		0, 0, rect.Width(), rect.Height(),
		imgTmp.data,
		&bitInfo,
		DIB_RGB_COLORS,
		SRCCOPY);
	ReleaseDC(pDC);

}

void CShowImage::DisplayImage(const Mat& imgData)
{
	m_showRect = Rect(0, 0, 0, 0);
	m_inMat.release();
	imgData.copyTo(m_inMat);

	RedrawWindow(NULL);
}

void CShowImage::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (m_inMat.empty())
		return;

	m_starPt.x = point.x;
	m_starPt.y = point.y;
	m_bLdown = true;
	CStatic::OnLButtonDown(nFlags, point);
}


void CShowImage::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (m_inMat.empty())
		return;

	if (m_bLdown){
		Point endPt;
		endPt.x = point.x;
		endPt.y = point.y;
		m_showRect = Rect(m_starPt, endPt);
		RedrawWindow(NULL);
	}
	CStatic::OnMouseMove(nFlags, point);	
}


void CShowImage::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (m_inMat.empty())
		return;

	Point endPt;
	endPt.x = point.x;
	endPt.y = point.y;

	m_showRect = Rect(m_starPt, endPt);
	m_bLdown = false;
	RedrawWindow(NULL);

	if (m_showRect.area() > 0)
		::SendMessage(GetParent()->GetSafeHwnd(), WM_SendRect, NULL, (LPARAM)&m_showRect);

	CStatic::OnLButtonUp(nFlags, point);
}
