
// TrafficLightDetection.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTrafficLightDetectionApp: 
// �йش����ʵ�֣������ TrafficLightDetection.cpp
//

class CTrafficLightDetectionApp : public CWinApp
{
public:
	CTrafficLightDetectionApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTrafficLightDetectionApp theApp;