
// TrafficLightDetectionDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "ShowImg.h"
#include "DisplayImg.h"
#include "Resource.h"

#include "TrafficLightIdentify.h"

// CTrafficLightDetectionDlg �Ի���
class CTrafficLightDetectionDlg : public CDialogEx
{
// ����
public:
	CTrafficLightDetectionDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TRAFFICLIGHTDETECTION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg LRESULT GetRect(WPARAM wparam, LPARAM lparam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButLoadimg();
	afx_msg void OnBnClickedButLocate();
	afx_msg void OnBnClickedButRecongnize();

	void CalcMatHist();
	void Getfilepath(const CString& strPath);
	void Getfolder();
	void DisplayTrafficLightsInfo(vector<vector<Rect>>& rectVec_trafficLights);
public:
	// display the detection results
	CListCtrl m_List_DisplayInfo;
	CShowImage m_ShowImg;
	CDisplayImg m_DisplayImg;

	vector<CString> m_Vstrfile;
	vector<Mat> m_vTemplatMat;

	Mat m_Img;
	int m_fileNum;
	bool m_bLoatTemplate;
	Mat m_templateMat;

	vector<vector<Rect>> rectVec_trafficLights;

	CTrafficLightIdentify m_trafficLightDetector;
	afx_msg void OnBnClickedButUpdatetemplate();
	
	afx_msg void OnBnClickedButUpdate();
	afx_msg void OnBnClickedButNext();
};
