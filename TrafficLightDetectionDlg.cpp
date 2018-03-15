
// TrafficLightDetectionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TrafficLightDetection.h"
#include "TrafficLightDetectionDlg.h"
#include "afxdialogex.h"
#include <atlconv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CTrafficLightDetectionDlg 对话框



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


// CTrafficLightDetectionDlg 消息处理程序
BOOL CTrafficLightDetectionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	m_fileNum = 0;
	GetDlgItem(IDC_BUT_NEXT)->ShowWindow(false);
	m_bLoatTemplate = false;

	CFileFind find;
	BOOL isFinded = find.FindFile(_T("templateMat.xml"));
	if (isFinded){
		FileStorage fs(".\\templateMat.xml", FileStorage::READ);
		fs["templateMat"] >> m_templateMat;
	}
	
	LPWSTR szColumn[5] = { _T("编号"), _T("图像路径"), _T("输出检测矩形框"), _T("信号灯颜色"), _T("时间") };
	m_List_DisplayInfo.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	for (int i = 0; i < 5; i++)                                                //插入各列
	{
		lvc.pszText = szColumn[i];
		if (i == 0)
			lvc.cx = m_List_DisplayInfo.GetStringWidth(szColumn[0]) + 50;                 //获取列的宽度
		else
			lvc.cx = m_List_DisplayInfo.GetStringWidth(szColumn[i]) + 80;                 //获取列的宽度

		lvc.iSubItem = i;
		m_List_DisplayInfo.InsertColumn(i, &lvc);
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTrafficLightDetectionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTrafficLightDetectionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTrafficLightDetectionDlg::OnBnClickedButLoadimg()
{
	m_Img.release();
	m_bLoatTemplate = false;
	// TODO:  在此添加控件通知处理程序代码
	CString fileter = _T("JPG files(*.jpg)|*.jpg|BMP files(*.bmp)|*.bmp|任何文件(*.*)|*.*||");
	CFileDialog filedlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, fileter);
	filedlg.m_ofn.lpstrTitle = _T("请选择要打开的图像文件");

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
		GetDlgItem(IDC_STATIC_ShowImg)->GetClientRect(&rect);  // 获取控件大小
		resize(m_Img, m_Img, Size(rect.Width(), rect.Height()));// 缩放Mat并备份
		
		//CalcMatHist();
		m_ShowImg.DisplayImage(m_Img);
	}
}


void CTrafficLightDetectionDlg::OnBnClickedButLocate()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_Img.empty())
		return;
	if (m_templateMat.empty())
		AfxMessageBox(_T("请先加载模板"));

	rectVec_trafficLights.clear();
	Mat redMat, greenMat;
	m_trafficLightDetector.extractTrafficLightsRects(m_Img, m_templateMat, rectVec_trafficLights, redMat, greenMat);

	m_ShowImg.DisplayImage(redMat);
	m_DisplayImg.DisplayImage(greenMat);
}


void CTrafficLightDetectionDlg::OnBnClickedButRecongnize()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_Img.empty())
		return;
	if (rectVec_trafficLights.empty()){
		AfxMessageBox(_T("请先点击 定位 按钮~"));
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
		CString  csFileName = m_Vstrfile[0].Right(m_Vstrfile[0].GetLength() - 1 - nPos); // 获取文件名
		m_List_DisplayInfo.SetItemText(iItemCount, 1, csFileName);

		char buf[80];
		sprintf_s(buf, "(x=%d, y=%d, width=%d, height=%d)", tmpRect.x, tmpRect.y, tmpRect.width, tmpRect.height);
		int num = MultiByteToWideChar(0, 0, buf, -1, NULL, 0);
		wchar_t *wide = new wchar_t[num];
		MultiByteToWideChar(0, 0, buf, -1, wide, num);

		m_List_DisplayInfo.SetItemText(iItemCount, 2, wide);
		m_List_DisplayInfo.SetItemText(iItemCount, 3, _T("红色"));

		CTime tTime = CTime::GetCurrentTime();
		CString strTemp = tTime.Format("%Y-%m-%d %H:%M:%S");
		//往第iItemCount插入第四列的数据数据，即插入登记时间
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
		CString  csFileName = m_Vstrfile[0].Right(m_Vstrfile[0].GetLength() - 1 - nPos); // 获取文件名
		m_List_DisplayInfo.SetItemText(iItemCount, 1, csFileName);

		char buf[80];
		sprintf_s(buf, "(x=%d, y=%d, width=%d, height=%d)", tmpRect.x, tmpRect.y, tmpRect.width, tmpRect.height);
		int num = MultiByteToWideChar(0, 0, buf, -1, NULL, 0);
		wchar_t *wide = new wchar_t[num];
		MultiByteToWideChar(0, 0, buf, -1, wide, num);

		m_List_DisplayInfo.SetItemText(iItemCount, 2, wide);
		m_List_DisplayInfo.SetItemText(iItemCount, 3, _T("绿色"));

		CTime tTime = CTime::GetCurrentTime();
		CString strTemp = tTime.Format("%Y-%m-%d %H:%M:%S");
		//往第iItemCount插入第四列的数据数据，即插入登记时间
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
	// TODO:  在此添加控件通知处理程序代码
	m_Img.release();
	m_vTemplatMat.clear();
	m_bLoatTemplate = true;
	AfxMessageBox(_T("更新模板步骤：\n 1. 选择包含交通信号灯图像的文件夹 \n 2. 挨个浏览图像，并用鼠标框选交通信号灯黑色边框 \n 3. 点击更新按钮，模板更新"));
	
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
	GetDlgItem(IDC_STATIC_ShowImg)->GetClientRect(&rect);  // 获取控件大小
	resize(m_Img, m_Img, Size(rect.Width(), rect.Height()));// 缩放Mat并备份

	//CalcMatHist();
	m_ShowImg.DisplayImage(m_Img);
}

void CTrafficLightDetectionDlg::OnBnClickedButNext()
{
	// TODO:  在此添加控件通知处理程序代码
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
	GetDlgItem(IDC_STATIC_ShowImg)->GetClientRect(&rect);  // 获取控件大小
	resize(m_Img, m_Img, Size(rect.Width(), rect.Height()));// 缩放Mat并备份

	//CalcMatHist();
	m_ShowImg.DisplayImage(m_Img);
}


void CTrafficLightDetectionDlg::OnBnClickedButUpdate()
{
	// TODO:  在此添加控件通知处理程序代码
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
	str.Format(_T("更新模板成功, %d 个特征向量; \n 以Mat的形式保存在当前路径，templateMat.xml"), m_vTemplatMat.size());
	AfxMessageBox(str);
}

void CTrafficLightDetectionDlg::Getfolder()
{
	CString strFolderPath(_T(""));
	TCHAR szPath[_MAX_PATH];

	BROWSEINFO bi;

	bi.hwndOwner = GetSafeHwnd();
	bi.pidlRoot = NULL;
	bi.lpszTitle = _T("选择包含交通信号灯图像的文件夹");
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

		// 防止内存泄露，要使用IMalloc接口
		IMalloc*pMalloc;

		if (SHGetMalloc(&pMalloc) != NOERROR)
		{
			TRACE(_T("无法取得外壳程序的IMalloc接口\n"));
		}

		pMalloc->Free(pItemIDList);

		if (pMalloc)
			pMalloc->Release();
	}
	else
	{
		strFolderPath = _T("");                                 // 文件夹路径为空
	}

	Getfilepath(strFolderPath);                               // 调用遍历文件夹函数
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

		// 如果是"." 则不扫描
		if (file.IsDots())
		{
			continue;
		}
		// 如果是是目录,继续扫描此目录
		else
		{
			if (file.IsDirectory())
			{
				strfile = file.GetFileName();
				strtemp = strtemp + _T("\\") + strfile;
				Getfilepath(strtemp);
			}
			// 文件
			else
			{
				strfile = file.GetFileName();

				m_Vstrfile.push_back(strtemp + _T("\\") + strfile);           //将文件路径压入容器


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
