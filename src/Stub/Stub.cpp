// Stub.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "Stub.h"


#pragma comment(linker, "/merge:.data=.text") 
#pragma comment(linker, "/merge:.rdata=.text")
#pragma comment(linker, "/section:.text,RWE")
extern "C"__declspec(dllexport) GLOBAL_PARAM g_stcParam = { (DWORD)(Start) };

typedef void(*FUN)();
FUN g_oep;

ULONGLONG GetKernel32Addr()
{
	ULONGLONG dwKernel32Addr = 0;

	// ��ȡTEB�ĵ�ַ
	_TEB* pTeb = NtCurrentTeb();
	// ��ȡPEB�ĵ�ַ
	PULONGLONG pPeb = (PULONGLONG)*(PULONGLONG)((ULONGLONG)pTeb + 0x60); 
	// ��ȡPEB_LDR_DATA�ṹ�ĵ�ַ
	PULONGLONG pLdr = (PULONGLONG)*(PULONGLONG)((ULONGLONG)pPeb + 0x18); 
	//ģ���ʼ�������ͷָ��InInitializationOrderModuleList
	PULONGLONG pInLoadOrderModuleList = (PULONGLONG)((ULONGLONG)pLdr + 0x10);
	// ��ȡ�����е�һ��ģ����Ϣ��exeģ��
	PULONGLONG pModuleExe = (PULONGLONG)*pInLoadOrderModuleList; 
	// ��ȡ�����еڶ���ģ����Ϣ��ntdllģ��
	PULONGLONG pModuleNtdll = (PULONGLONG)*pModuleExe;    
	// ��ȡ�����е�����ģ����Ϣ��Kernel32ģ��
	PULONGLONG pModuleKernel32 = (PULONGLONG)*pModuleNtdll;   
	// ��ȡkernel32��ַ
	dwKernel32Addr = pModuleKernel32[6];                     
	return dwKernel32Addr;
}

ULONGLONG MyGetProcAddress()
{
	ULONGLONG dwBase = GetKernel32Addr();
	// 1. ��ȡDOSͷ
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)dwBase;
	// 2. ��ȡNTͷ
	PIMAGE_NT_HEADERS64  pNt = (PIMAGE_NT_HEADERS64)(dwBase + pDos->e_lfanew);
	// 3. ��ȡ����Ŀ¼��
	PIMAGE_DATA_DIRECTORY pExportDir = pNt->OptionalHeader.DataDirectory;
	pExportDir = &(pExportDir[IMAGE_DIRECTORY_ENTRY_EXPORT]);
	DWORD dwOffset = pExportDir->VirtualAddress;
	// 4. ��ȡ��������Ϣ�ṹ
	PIMAGE_EXPORT_DIRECTORY pExport = (PIMAGE_EXPORT_DIRECTORY)(dwBase + dwOffset);
	DWORD dwFunCount = pExport->NumberOfFunctions;
	DWORD dwFunNameCount = pExport->NumberOfNames;
	DWORD dwModOffset = pExport->Name;

	// Get Export Address Table
	PDWORD pEAT = (PDWORD)(dwBase + pExport->AddressOfFunctions);
	// Get Export Name Table
	PDWORD pENT = (PDWORD)(dwBase + pExport->AddressOfNames);
	// Get Export Index Table
	PWORD  pEIT = (PWORD)(dwBase + pExport->AddressOfNameOrdinals);

	for (DWORD dwOrdinal = 0; dwOrdinal < dwFunCount; dwOrdinal++)
	{
		if (!pEAT[dwOrdinal]) // Export Address offset
			continue;

		// 1. ��ȡ���
		DWORD dwID = pExport->Base + dwOrdinal;
		// 2. ��ȡ����������ַ
		DWORD dwFunAddrOffset = pEAT[dwOrdinal];

		for (DWORD dwIndex = 0; dwIndex < dwFunNameCount; dwIndex++)
		{
			// ����ű��в��Һ��������
			if (pEIT[dwIndex] == dwOrdinal)
			{
				// ��������������������Ʊ��е�����
				DWORD dwNameOffset = pENT[dwIndex];
				char* pFunName = (char*)((DWORD)dwBase + dwNameOffset);
				if (!strcmp(pFunName, "GetProcAddress"))
				{// ���ݺ������Ʒ��غ�����ַ
					return dwBase + dwFunAddrOffset;
				}
			}
		}
	}
	return 0;
}

void XorCode()
{
	PBYTE pBase = (PBYTE)((ULONGLONG)g_stcParam.dwImageBase + g_stcParam.lpStartVA);

	for (DWORD i = 0; i < g_stcParam.dwCodeSize; i++)
	{
		pBase[i] ^= g_stcParam.byXor;
	}
}

void  Start()
{
	fnGetProcAddress pfnGetProcAddress = (fnGetProcAddress)MyGetProcAddress();

	ULONGLONG dwBase = GetKernel32Addr();
	fnLoadLibraryA pfnLoadLibraryA = (fnLoadLibraryA)pfnGetProcAddress((HMODULE)dwBase, "LoadLibraryA");
	fnGetModuleHandleA pfnGetModuleHandleA = (fnGetModuleHandleA)pfnGetProcAddress((HMODULE)dwBase, "GetModuleHandleA");
	fnVirtualProtect pfnVirtualProtect = (fnVirtualProtect)pfnGetProcAddress((HMODULE)dwBase, "VirtualProtect");
	fnMessageBox pfnMessageBoxA = (fnMessageBox)pfnGetProcAddress(pfnGetModuleHandleA("user32.dll"), "MessageBoxA");
	fnExitProcess pfnExitProcess = (fnExitProcess)pfnGetProcAddress(pfnGetModuleHandleA("kernel32.dll"), "ExitProcess");
	ULONGLONG dwCodeBase = g_stcParam.dwImageBase + (DWORD)g_stcParam.lpStartVA;
	DWORD dwOldProtect = 0;
	pfnVirtualProtect((LPBYTE)dwCodeBase, g_stcParam.dwCodeSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	XorCode();
	pfnVirtualProtect((LPBYTE)dwCodeBase, g_stcParam.dwCodeSize, dwOldProtect, &dwOldProtect);

	int nRet = pfnMessageBoxA(NULL, "��ӭʹ�����64λ�ӿǳ����Ƿ�����������", "Hello 15PB", MB_YESNO);
	if (nRet == IDYES)
	{
		g_oep = (FUN)(g_stcParam.dwImageBase + g_stcParam.dwOEP);
		g_oep();
	}
	pfnExitProcess(0);
}

