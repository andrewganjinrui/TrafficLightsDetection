
// TrafficLightDetectionDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TrafficLightDetection.h"
#include "TrafficLightDetectionDlg.h"
#include "afxdialogex.h"
#include <atlconv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTrafficLightDetectionDlg �Ի���



CTrafficLightDetectionDlg::CTrafficLightDetectionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTrafficLightDetectionDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTrafficLightDetectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_LIST_DisplayInfo, m_list_DisplayInfo);
	DDX_Control(pDX, IDC_LIST_DisplayInfo, m_List_DisplayInfo);
	DDX_Control(pDX, IDC_STATIC_ShowImg, m_ShowImg);
	DDX_Control(pDX, IDC_STATIC_ShowResultImg, m_DisplayImg);
}

BEGIN_MESSAGE_MAP(CTrafficLightDetectionDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUT_LoadImg, &CTrafficLightDetectionDlg::OnBnClickedButLoadimg)
	ON_BN_CLICKED(IDC_BUT_Locate, &CTrafficLightDetectionDlg::OnBnClickedButLocate)
	ON_BN_CLICKED(IDC_BUT_Recongnize, &CTrafficLightDetectionDlg::OnBnClickedButRecongnize)
	ON_BN_CLICKED(IDC_BUT_UpdateTemplate, &CTrafficLightDetectionDlg::OnBnClickedButUpdatetemplate)
	ON_BN_CLICKED(IDC_BUT_Update, &CTrafficLightDetectionDlg::OnBnClickedButUpdate)
	ON_BN_CLICKED(IDC_BUT_NEXT, &CTrafficLightDetectionDlg::OnBnClickedButNext)

	ON_MESSAGE(WM_SendRect, GetRect)
END_MESSAGE_MAP()


// CTrafficLightDetectionDlg ��Ϣ�������
BOOL CTrafficLightDetectionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	m_fileNum = 0;
	GetDlgItem(IDC_BUT_NEXT)->ShowWindow(false);
	m_bLoatTemplate = false;

	CFileFind find;
	BOOL isFinded = find.FindFile(_T("templateMat.xml"));
	if (isFinded){
		FileStorage fs(".\\templateMat.xml", FileStorage::READ);
		fs["templateMat"] >> m_templateMat;
	}
	
	LPWSTR szColumn[5] = { _T("���"), _T("ͼ��·��"), _T("��������ο�"), _T("�źŵ���ɫ"), _T("ʱ��") };
	m_List_DisplayInfo.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	for (int i = 0; i < 5; i++)                                                //�������
	{
		lvc.pszText = szColumn[i];
		if (i == 0)
			lvc.cx = m_List_DisplayInfo.GetStringWidth(szColumn[0]) + 50;                 //��ȡ�еĿ��
		else
			lvc.cx = m_List_DisplayInfo.GetStringWidth(szColumn[i]) + 80;                 //��ȡ�еĿ��

		lvc.iSubItem = i;
		m_List_DisplayInfo.InsertColumn(i, &lvc);
	}

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CTrafficLightDetectionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTrafficLightDetectionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CTrafficLightDetectionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTrafficLightDetectionDlg::OnBnClickedButLoadimg()
{
	m_Img.release();
	m_bLoatTemplate = false;
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString fileter = _T("JPG files(*.jpg)|*.jpg|BMP files(*.bmp)|*.bmp|�κ��ļ�(*.*)|*.*||");
	CFileDialog filedlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, fileter);
	filedlg.m_ofn.lpstrTitle = _T("��ѡ��Ҫ�򿪵�ͼ���ļ�");

	if (IDOK == filedlg.DoModal())
	{
		m_Vstrfile.clear();

		CString filename = filedlg.GetPathName();
		m_Vstrfile.push_back(filename);

		USES_CONVERSION;
		string str(W2A(filename));

		m_Img = imread(str, CV_LOAD_IMAGE_UNCHANGED);
		if (m_Img.empty())
			return;

		CRect rect;
		GetDlgItem(IDC_STATIC_ShowImg)->GetClientRect(&rect);  // ��ȡ�ؼ���С
		resize(m_Img, m_Img, Size(rect.Width(), rect.Height()));// ����Mat������
		
		//CalcMatHist();
		m_ShowImg.DisplayImage(m_Img);
	}
}


void CTrafficLightDetectionDlg::OnBnClickedButLocate()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (m_Img.empty())
		return;
	if (m_templateMat.empty())
		AfxMessageBox(_T("���ȼ���ģ��"));

	rectVec_trafficLights.clear();
	Mat redMat, greenMat;
	m_trafficLightDetector.extractTrafficLightsRects(m_Img, m_templateMat, rectVec_trafficLights, redMat, greenMat);

	m_ShowImg.DisplayImage(redMat);
	m_DisplayImg.DisplayImage(greenMat);
}


void CTrafficLightDetectionDlg::OnBnClickedButRecongnize()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (m_Img.empty())
		return;
	if (rectVec_trafficLights.empty()){
		AfxMessageBox(_T("���ȵ�� ��λ ��ť~"));
		return;
	}
	
	Mat resultMat;
	m_trafficLightDetector.draw_showResultImg(resultMat, rectVec_trafficLights);

	m_ShowImg.DisplayImage(m_Img);
	m_DisplayImg.DisplayImage(resultMat);
	
	DisplayTrafficLightsInfo(rectVec_trafficLights);
	rectVec_trafficLights.clear();
}

void CTrafficLightDetectionDlg::DisplayTrafficLightsInfo(vector<vector<Rect>>& rectVec_trafficLights)
{
	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT;
	UpdateData();
	Rect tmpRect;
	int iItemCount;
	CString str;
	vector<Rect> trafficRedLightRect = rectVec_trafficLights[0];
	for (int i = 0; i < trafficRedLightRect.size(); i++) {
		tmpRect = trafficRedLightRect[i];
		//rectangle(img_result, tmpRect, Scalar(0, 0, 255), 3);
		iItemCount = m_List_DisplayInfo.GetItemCount();
		m_List_DisplayInfo.InsertItem(iItemCount, _T(""));
		str.Format(_T("%d"), iItemCount+1);
		m_List_DisplayInfo.SetItemText(iItemCount, 0, str);

		int nPos = m_Vstrfile[0].ReverseFind('\\');
		CString  csFileName = m_Vstrfile[0].Right(m_Vstrfile[0].GetLength() - 1 - nPos); // ��ȡ�ļ���
		m_List_DisplayInfo.SetItemText(iItemCount, 1, csFileName);

		char buf[80];
		sprintf_s(buf, "(x=%d, y=%d, width=%d, height=%d)", tmpRect.x, tmpRect.y, tmpRect.width, tmpRect.height);
		int num = MultiByteToWideChar(0, 0, buf, -1, NULL, 0);
		wchar_t *wide = new wchar_t[num];
		MultiByteToWideChar(0, 0, buf, -1, wide, num);

		m_List_DisplayInfo.SetItemText(iItemCount, 2, wide);
		m_List_DisplayInfo.SetItemText(iItemCount, 3, _T("��ɫ"));

		CTime tTime = CTime::GetCurrentTime();
		CString strTemp = tTime.Format("%Y-%m-%d %H:%M:%S");
		//����iItemCount��������е��������ݣ�������Ǽ�ʱ��
		m_List_DisplayInfo.SetItemText(iItemCount, 4, strTemp);
	}

	vector<Rect> trafficGreenLightRect = rectVec_trafficLights[1];
	for (int i = 0; i < trafficGreenLightRect.size(); i++) {
		tmpRect = trafficGreenLightRect[i];
		//rectangle(img_result, tmpRect, Scalar(0, 255, 0), 3);
		iItemCount = m_List_DisplayInfo.GetItemCount();
		m_List_DisplayInfo.InsertItem(iItemCount, _T(""));
		str.Format(_T("%d"), iItemCount+1);
		m_List_DisplayInfo.SetItemText(iItemCount, 0, str);

		int nPos = m_Vstrfile[0].ReverseFind('\\');
		CString  csFileName = m_Vstrfile[0].Right(m_Vstrfile[0].GetLength() - 1 - nPos); // ��ȡ�ļ���
		m_List_DisplayInfo.SetItemText(iItemCount, 1, csFileName);

		char buf[80];
		sprintf_s(buf, "(x=%d, y=%d, width=%d, height=%d)", tmpRect.x, tmpRect.y, tmpRect.width, tmpRect.height);
		int num = MultiByteToWideChar(0, 0, buf, -1, NULL, 0);
		wchar_t *wide = new wchar_t[num];
		MultiByteToWideChar(0, 0, buf, -1, wide, num);

		m_List_DisplayInfo.SetItemText(iItemCount, 2, wide);
		m_List_DisplayInfo.SetItemText(iItemCount, 3, _T("��ɫ"));

		CTime tTime = CTime::GetCurrentTime();
		CString strTemp = tTime.Format("%Y-%m-%d %H:%M:%S");
		//����iItemCount��������е��������ݣ�������Ǽ�ʱ��
		m_List_DisplayInfo.SetItemText(iItemCount, 4, strTemp);
	}
}

void CTrafficLightDetectionDlg::CalcMatHist()
{
	// TODO: Add your control notification handler code here

	Mat result;
	cvtColor(m_Img, result, CV_BGR2HSV);

	// Quantize the hue to 30 levels
	// and the saturation to 32 levels
	int hbins = 30, sbins = 32;
	int histSize[] = { hbins, sbins };
	// hue varies from 0 to 179, see cvtColor
	float hranges[] = { 0, 180 };
	// saturation varies from 0 (black-gray-white) to
	// 255 (pure spectrum color)
	float sranges[] = { 0, 256 };
	const float* ranges[] = { hranges, sranges };
	MatND hist;
	// we compute the histogram from the 0-th and 1-st channels
	int channels[] = { 0, 1 };

	calcHist(&result, 1, channels, Mat(), // do not use mask
		hist, 2, histSize, ranges,
		true, // the histogram is uniform
		false);
	double maxVal = 0;
	minMaxLoc(hist, 0, &maxVal, 0, 0);

	int scale = 10;
	Mat histImg = Mat::zeros(sbins*scale, hbins * 10, CV_8UC3);

	for (int h = 0; h < hbins; h++) {
		for (int s = 0; s < sbins; s++) {
			float binVal = hist.at<float>(h, s);
			int intensity = cvRound(binVal * 255 / maxVal);
			rectangle(histImg, Point(h*scale, s*scale),
				Point((h + 1)*scale - 1, (s + 1)*scale - 1),
				Scalar::all(intensity),
				CV_FILLED);
		}
	}

	namedWindow("H-S Histogram", 1);
	imshow("H-S Histogram", histImg);
}


void CTrafficLightDetectionDlg::OnBnClickedButUpdatetemplate()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_Img.release();
	m_vTemplatMat.clear();
	m_bLoatTemplate = true;
	AfxMessageBox(_T("����ģ�岽�裺\n 1. ѡ�������ͨ�źŵ�ͼ����ļ��� \n 2. �������ͼ�񣬲�������ѡ��ͨ�źŵƺ�ɫ�߿� \n 3. ������°�ť��ģ�����"));
	
	m_Vstrfile.clear();
	Getfolder();

	if (m_Vstrfile.empty())
		return;

	if (m_Vstrfile.size() > 1)
		GetDlgItem(IDC_BUT_NEXT)->ShowWindow(true);

	CString filename = m_Vstrfile[0];
	USES_CONVERSION;
	string str(W2A(filename));

	m_Img = imread(str, CV_LOAD_IMAGE_UNCHANGED);
	if (m_Img.empty())
		return;

	CRect rect;
	GetDlgItem(IDC_STATIC_ShowImg)->GetClientRect(&rect);  // ��ȡ�ؼ���С
	resize(m_Img, m_Img, Size(rect.Width(), rect.Height()));// ����Mat������

	//CalcMatHist();
	m_ShowImg.DisplayImage(m_Img);
}

void CTrafficLightDetectionDlg::OnBnClickedButNext()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_Img.release();
	++m_fileNum;
	if (m_fileNum >= m_Vstrfile.size())
		return;
	if (m_fileNum == m_Vstrfile.size()-1)
		GetDlgItem(IDC_BUT_NEXT)->ShowWindow(false);

	CString filename = m_Vstrfile[m_fileNum];
	USES_CONVERSION;
	string str(W2A(filename));

	m_Img = imread(str, CV_LOAD_IMAGE_UNCHANGED);
	if (m_Img.empty())
		return;

	CRect rect;
	GetDlgItem(IDC_STATIC_ShowImg)->GetClientRect(&rect);  // ��ȡ�ؼ���С
	resize(m_Img, m_Img, Size(rect.Width(), rect.Height()));// ����Mat������

	//CalcMatHist();
	m_ShowImg.DisplayImage(m_Img);
}


void CTrafficLightDetectionDlg::OnBnClickedButUpdate()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_bLoatTemplate = false;
	m_templateMat = m_vTemplatMat[0];
	for (int i = 1; i < m_vTemplatMat.size(); i++)
	{
		m_trafficLightDetector.mergeRows(m_templateMat, m_vTemplatMat[i]);
	}

	FileStorage fs(".\\templateMat.xml", FileStorage::WRITE);
	fs << "templateMat" << m_templateMat;
	fs.release();

	CString str;
	str.Format(_T("����ģ��ɹ�, %d ����������; \n ��Mat����ʽ�����ڵ�ǰ·����templateMat.xml"), m_vTemplatMat.size());
	AfxMessageBox(str);
}

void CTrafficLightDetectionDlg::Getfolder()
{
	CString strFolderPath(_T(""));
	TCHAR szPath[_MAX_PATH];

	BROWSEINFO bi;

	bi.hwndOwner = GetSafeHwnd();
	bi.pidlRoot = NULL;
	bi.lpszTitle = _T("ѡ�������ͨ�źŵ�ͼ����ļ���");
	bi.pszDisplayName = szPath;
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = NULL;
	bi.iImage = NULL;

	LPITEMIDLIST pItemIDList = SHBrowseForFolder(&bi);

	if (pItemIDList)
	{
		if (SHGetPathFromIDList(pItemIDList, szPath))
		{
			strFolderPath = szPath;
		}

		// ��ֹ�ڴ�й¶��Ҫʹ��IMalloc�ӿ�
		IMalloc*pMalloc;

		if (SHGetMalloc(&pMalloc) != NOERROR)
		{
			TRACE(_T("�޷�ȡ����ǳ����IMalloc�ӿ�\n"));
		}

		pMalloc->Free(pItemIDList);

		if (pMalloc)
			pMalloc->Release();
	}
	else
	{
		strFolderPath = _T("");                                 // �ļ���·��Ϊ��
	}

	Getfilepath(strFolderPath);                               // ���ñ����ļ��к���
}

void CTrafficLightDetectionDlg::Getfilepath(const CString &strPath)
{
	CFileFind  file;
	CString strfile;
	CString strtemp = strPath;
	CString   strDirectory = strPath + _T("\\") + _T("\\*.*");


	BOOL IsFind = file.FindFile(strDirectory);
	while (IsFind)
	{
		IsFind = file.FindNextFile();

		// �����"." ��ɨ��
		if (file.IsDots())
		{
			continue;
		}
		// �������Ŀ¼,����ɨ���Ŀ¼
		else
		{
			if (file.IsDirectory())
			{
				strfile = file.GetFileName();
				strtemp = strtemp + _T("\\") + strfile;
				Getfilepath(strtemp);
			}
			// �ļ�
			else
			{
				strfile = file.GetFileName();

				m_Vstrfile.push_back(strtemp + _T("\\") + strfile);           //���ļ�·��ѹ������


			}
		}
	}

	file.Close();
}

LRESULT CTrafficLightDetectionDlg::GetRect(WPARAM wparam, LPARAM lparam)
{
	Rect showRect = *((Rect*)lparam);
	if (!m_bLoatTemplate)
		return 0;

	m_trafficLightDetector.setMat(m_Img);  //set m_inMat

	Mat equaled_img;
	m_trafficLightDetector.Preprocess_equalizeHist(equaled_img);  //set equaled_img

	MatND histMat;
	m_trafficLightDetector.calcMatROIHist(equaled_img, showRect, histMat);

	m_vTemplatMat.push_back(histMat);
	//rectangle(m_Img, showRect, Scalar(0, 0, 255), 2);
	//m_DisplayImg.DisplayImage(m_Img);


	//Mat mat = Mat::eye(Size(12, 12), CV_8UC1);
	//FileStorage fs(".\\vocabulary.xml", FileStorage::WRITE);
	//fs << "vocabulary" << mat;
	//fs.release();

	//FileStorage fs(".\\vocabulary.xml", FileStorage::READ);
	//Mat mat_vocabulary;
	//fs["vocabulary"] >> m_templateMat;

	return 0;
}
