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
	// TODO:  �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CStatic::OnPaint()

	if (m_inMat.empty())
		return;

	Mat imgTmp;
	CRect rect;
	GetClientRect(&rect);  // ��ȡ�ؼ���С
	resize(m_inMat, imgTmp, Size(rect.Width(), rect.Height()));// ����Mat������


	// תһ�¸�ʽ,��ο��Է�����,
	switch (imgTmp.channels())
	{
	case 1:
		cvtColor(imgTmp, imgTmp, CV_GRAY2BGRA); // GRAY��ͨ��
		break;
	case 3:
		cvtColor(imgTmp, imgTmp, CV_BGR2BGRA);  // BGR��ͨ��
		break;
	default:
		break;
	}

	int pixelBytes = imgTmp.channels()*(imgTmp.depth() + 1); // ����һ�����ض��ٸ��ֽ�
	// ����bitmapinfo(����ͷ)
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

	// Mat.data+ bitmap����ͷ -> MFC
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
