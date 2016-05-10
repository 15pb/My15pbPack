// Pack.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "Pack.h"


// ���ǵ���������һ��ʾ��
PACK_API int nPack=0;

// ���ǵ���������һ��ʾ����
PACK_API int fnPack(void)
{
	return 42;
}

// �����ѵ�����Ĺ��캯����
// �й��ඨ�����Ϣ������� Pack.h
CPack::CPack()
{
	return;
}

#include <Psapi.h>
#pragma comment(lib,"psapi.lib")
#include "../Stub/Stub.h"
#ifdef DEBUG
#pragma comment(lib,"../x64/Debug/Stub.lib")
#endif // DEBUG
#ifdef NDEBUG
#pragma comment(lib,"../x64/release/Stub.lib")
#endif // NDEBUG

#include "PE.h"
BOOL Pack(CString strPath, BYTE byXor)
{
	// 1.��ȡĿ���ļ�PE��Ϣ
	CPE objPE;
	objPE.InitPE(strPath);
	BOOL bRet = FALSE;
	DWORD dwVirtualAddr = objPE.XorCode(0x15);

	//2. ��ȡStub�ļ�PE��Ϣ,����Ҫ����Ϣ���õ�Stub��
	HMODULE hMod = LoadLibrary(L"Stub.dll");
	PGLOBAL_PARAM pstcParam = (PGLOBAL_PARAM)GetProcAddress(hMod, "g_stcParam");
	pstcParam->dwImageBase = objPE.m_dwImageBase;
	pstcParam->dwCodeSize = objPE.m_dwCodeSize;
	pstcParam->dwOEP = objPE.m_dwOEP;
	pstcParam->byXor = byXor;
	pstcParam->lpStartVA = (PBYTE)dwVirtualAddr;

	// 3. ���Stub����ε����ӿǳ�����
	// 3.1 ��ȡStub�����
	MODULEINFO modinfo = { 0 };
	GetModuleInformation(GetCurrentProcess(), hMod, &modinfo, sizeof(MODULEINFO));
	PBYTE  lpMod = new BYTE[modinfo.SizeOfImage];
	memcpy_s(lpMod, modinfo.SizeOfImage, hMod, modinfo.SizeOfImage);
	PBYTE pCodeSection = NULL;
	DWORD dwCodeBaseRVA = 0;
	DWORD dwSize = objPE.GetSectionData(lpMod, 0, pCodeSection, dwCodeBaseRVA);

	// 3.2 �޸�Stub���еĴ���
	objPE.FixReloc(lpMod, pCodeSection, objPE.m_dwNewSectionRVA);

	// 3.3 �޸ı��ӿǳ����OEP��ָ��stub
	DWORD dwStubOEPRVA = pstcParam->dwStart - (DWORD)hMod;
	DWORD dwNewOEP = dwStubOEPRVA - dwCodeBaseRVA;
	//StubOEP = dwStubOEPRVA - ԭRVA + �����ε�RVA;
	objPE.SetNewOEP(dwNewOEP);
	objPE.ClearRandBase();
	objPE.ClearBundleImport();//�����а��������ĳ���

	// 3.4 ��ȡ�ض�λ����Ϣ���޸�����
	if (objPE.AddSection(pCodeSection, dwSize, "15pack"))
	{
		bRet = TRUE;
	}
	// �ͷ���Դ
	delete lpMod;
	lpMod = NULL;
	return bRet;
}