
// My15pbPack.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMy15pbPackApp: 
// �йش����ʵ�֣������ My15pbPack.cpp
//

class CMy15pbPackApp : public CWinApp
{
public:
	CMy15pbPackApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMy15pbPackApp theApp;