
// My15pbPackDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "My15pbPack.h"
#include "My15pbPackDlg.h"
#include "afxdialogex.h"

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


// CMy15pbPackDlg �Ի���



CMy15pbPackDlg::CMy15pbPackDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMy15pbPackDlg::IDD, pParent)
	, m_strPath(_T(""))
	, m_strKey(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy15pbPackDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, EDT_TEXT, m_strPath);
	DDX_Text(pDX, EDT_KEY, m_strKey);
}

BEGIN_MESSAGE_MAP(CMy15pbPackDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(BTN_OPENFILE, &CMy15pbPackDlg::OnBnClickedOpenfile)
	ON_BN_CLICKED(BTN_PACK, &CMy15pbPackDlg::OnBnClickedPack)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CMy15pbPackDlg ��Ϣ�������

BOOL CMy15pbPackDlg::OnInitDialog()
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
	m_strKey = L"15";
	UpdateData(FALSE);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMy15pbPackDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMy15pbPackDlg::OnPaint()
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
HCURSOR CMy15pbPackDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMy15pbPackDlg::OnBnClickedOpenfile()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);

	static TCHAR BASED_CODE szFilter[] = _T("��ִ���ļ� (*.exe)|*.exe|")
		_T("DLL�ļ� (*.dll)|*.dll|exe Files (*.exe;*.exe)|")
		_T("*.exe; *.exe|All Files (*.*)|*.*||");

	CFileDialog fileDlg(TRUE, _T("my"), _T(""),
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilter);

	fileDlg.DoModal();

	m_strPath = fileDlg.GetPathName();

	UpdateData(FALSE);
}

#include "../Pack/Pack.h"
#ifdef DEBUG
#pragma comment(lib,"../x64/Debug/Pack.lib")
#endif // DEBUG
#ifdef NDEBUG
#pragma comment(lib,"../x64/release/Pack.lib")
#endif // NDEBUG


void CMy15pbPackDlg::OnBnClickedPack()
{
	//���ؼ�����Ϣ���µ�������
	UpdateData(TRUE);
	// ��ȡ����Key���ı�
	TCHAR szKey[20] = { 0 };
	_tcscpy_s(szKey, 20, m_strKey.GetBuffer());
	ULONGLONG nKey = 0;
	// ת��keyΪ16����
	_stscanf_s(szKey, L"%p", &nKey);
	// ����Packģ��
	if (Pack(m_strPath, (BYTE)nKey))
	{
		AfxMessageBox(L"�ӿǳɹ�");
	}
	//�������е���Ϣ���µ��ؼ���
	UpdateData(FALSE);
}

void CMy15pbPackDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	TCHAR szFilePath[MAX_PATH+2] = {0};
	DragQueryFile(hDropInfo, 0, szFilePath, _MAX_PATH);  //��ȡ��ק���ļ���
	DragFinish(hDropInfo);
	m_strPath = szFilePath;  // ��ֵ����Ա����
	UpdateData(FALSE);
	CDialogEx::OnDropFiles(hDropInfo);
}
