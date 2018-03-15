#include "stdafx.h"
#include "DisplayImg.h"


CDisplayImg::CDisplayImg()
{
}


CDisplayImg::~CDisplayImg()
{
}
BEGIN_MESSAGE_MAP(CDisplayImg, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()


void CDisplayImg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO:  在此处添加消息处理程序代码
	// 不为绘图消息调用 CStatic::OnPaint()

	if (m_inMat.empty())
		return;

	Mat imgTmp;
	CRect rect;
	GetClientRect(&rect);  // 获取控件大小
	resize(m_inMat, imgTmp, Size(rect.Width(), rect.Height()));// 缩放Mat并备份


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

void CDisplayImg::DisplayImage(const Mat& imgData)
{
	m_inMat.release();
	imgData.copyTo(m_inMat);

	RedrawWindow(NULL);
}
