#include "stdafx.h"
#include "PE.h"


CPE::CPE() :m_dwFileSize(0), m_pFileBase(NULL),
m_dwFileAlign(0), m_dwMemAlign(0)
{
}


CPE::~CPE()
{
}

DWORD CPE::RVA2OffSet(DWORD dwRVA, PIMAGE_NT_HEADERS64  pNt)
{
	DWORD dwOffset = 0;
	// 1. ��ȡ��һ�����νṹ��
	PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);
	// 2. ��ȡ��������
	DWORD dwSectionCount = pNt->FileHeader.NumberOfSections;
	// 3. ����������Ϣ��
	for (DWORD i = 0; i < dwSectionCount; i++)
	{
		// 4. ƥ��RVA���ڵ�����
		if (dwRVA >= pSection[i].VirtualAddress &&
			dwRVA < (pSection[i].VirtualAddress + pSection[i].Misc.VirtualSize)
			)
		{   // �����Ӧ���ļ�ƫ��
			dwOffset = dwRVA - pSection[i].VirtualAddress +
				pSection[i].PointerToRawData;
			return dwOffset;
		}
	}
	return dwOffset;
}


BOOL CPE::InitPE(CString strPath)
{
	if (m_objFile.m_hFile == INVALID_HANDLE_VALUE && m_pFileBase) {
		m_objFile.Close(); delete m_pFileBase; m_pFileBase = NULL;
	}

	m_objFile.Open(strPath, CFile::modeRead);
	m_dwFileSize = (DWORD)m_objFile.GetLength();
	m_pFileBase = new BYTE[m_dwFileSize];
	if (m_objFile.Read(m_pFileBase, (DWORD)m_dwFileSize))
	{
		// 1. ��ȡDOSͷ
		PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)m_pFileBase;
		// 2. ��ȡNTͷ
		m_pNT = (PIMAGE_NT_HEADERS64)((ULONGLONG)m_pFileBase + pDos->e_lfanew);
		// 3. ��ȡ�ļ����롢�ڴ�������Ϣ
		m_dwFileAlign = m_pNT->OptionalHeader.FileAlignment;
		m_dwMemAlign = m_pNT->OptionalHeader.SectionAlignment;
		m_dwImageBase = m_pNT->OptionalHeader.ImageBase;
		m_dwOEP = m_pNT->OptionalHeader.AddressOfEntryPoint;
		m_dwCodeBase = m_pNT->OptionalHeader.BaseOfCode;
		m_dwCodeSize = m_pNT->OptionalHeader.SizeOfCode;


		// 3. ��ȡ���һ�����κ�ĵ�ַ
		PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(m_pNT);
		m_pLastSection = &pSection[m_pNT->FileHeader.NumberOfSections];

		// 4. ��ȡ�¼����ε���ʼRVA,  �ڴ�4K����
		DWORD dwVirtualSize = m_pLastSection[-1].Misc.VirtualSize;
		if (dwVirtualSize%m_dwMemAlign)
			dwVirtualSize = (dwVirtualSize / m_dwMemAlign + 1) * m_dwMemAlign;
		else
			dwVirtualSize = (dwVirtualSize / m_dwMemAlign)* m_dwMemAlign;
		m_dwNewSectionRVA = m_pLastSection[-1].VirtualAddress + dwVirtualSize;

		return TRUE;
	}
	return FALSE;
}

ULONGLONG CPE::AddSection(LPBYTE pBuffer, DWORD dwSize, PCHAR pszSectionName)
{
	// 1 �޸��ļ�ͷ�е���������
	m_pNT->FileHeader.NumberOfSections++;

	// 2 �������α���
	memset(m_pLastSection, 0, sizeof(IMAGE_SECTION_HEADER));
	strcpy_s((char*)m_pLastSection->Name, IMAGE_SIZEOF_SHORT_NAME, pszSectionName);

	DWORD dwVirtualSize = 0; // ���������С
	DWORD dwSizeOfRawData = 0; // �����ļ���С
	DWORD dwSizeOfImage = m_pNT->OptionalHeader.SizeOfImage;
	{
		if (dwSizeOfImage%m_dwMemAlign)
			dwSizeOfImage = (dwSizeOfImage / m_dwMemAlign + 1) * m_dwMemAlign;
		else
			dwSizeOfImage = (dwSizeOfImage / m_dwMemAlign) * m_dwMemAlign;

		if (dwSize%m_dwMemAlign)
			dwVirtualSize = (dwSize / m_dwMemAlign + 1) * m_dwMemAlign;
		else
			dwVirtualSize = (dwSize / m_dwMemAlign) * m_dwMemAlign;

		if (dwSize%m_dwFileAlign)
			dwSizeOfRawData = (dwSize / m_dwFileAlign + 1) * m_dwFileAlign;
		else
			dwSizeOfRawData = (dwSize / m_dwFileAlign) * m_dwFileAlign;
	}

	m_pLastSection->VirtualAddress = dwSizeOfImage;
	m_pLastSection->PointerToRawData = m_dwFileSize;
	m_pLastSection->SizeOfRawData = dwSizeOfRawData;
	m_pLastSection->Misc.VirtualSize = dwVirtualSize;
	m_pLastSection->Characteristics = 0XE0000040;

	// 3 �����ļ���С,�����ļ�����Ӵ���
	m_pNT->OptionalHeader.SizeOfImage = dwSizeOfImage + dwVirtualSize;
	m_pNT->OptionalHeader.AddressOfEntryPoint = m_dwNewOEP + m_pLastSection->VirtualAddress;

	// 3.1 ��������ļ�·��
	CString strPath = m_objFile.GetFilePath();
	TCHAR szOutPath[MAX_PATH] = { 0 };
	LPWSTR strSuffix = PathFindExtension(strPath);                     // ��ȡ�ļ��ĺ�׺��
	wcsncpy_s(szOutPath, MAX_PATH, strPath, wcslen(strPath)); // ����Ŀ���ļ�·����szOutPath
	PathRemoveExtension(szOutPath);                                         // ��szOutPath�б���·���ĺ�׺��ȥ��
	wcscat_s(szOutPath, MAX_PATH, L"_15Pack");                       // ��·����󸽼ӡ�_15Pack��
	wcscat_s(szOutPath, MAX_PATH, strSuffix);                           // ��·����󸽼Ӹոձ���ĺ�׺��

	// 3.2 �����ļ�
	CFile objFile(szOutPath, CFile::modeCreate | CFile::modeReadWrite);
	objFile.Write(m_pFileBase, m_dwFileSize);
	objFile.SeekToEnd();
	objFile.Write(pBuffer, dwSize);

	return m_pLastSection->VirtualAddress;
}

// ���ڴ����ض�λStub
void CPE::FixReloc(PBYTE lpImage, PBYTE lpCode, DWORD dwCodeRVA)
{
	// 1. ��ȡDOSͷ
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)lpImage;
	// 2. ��ȡNTͷ
	PIMAGE_NT_HEADERS64  pNt = (PIMAGE_NT_HEADERS64)((ULONGLONG)lpImage + pDos->e_lfanew);
	// 3. ��ȡ����Ŀ¼��
	PIMAGE_DATA_DIRECTORY pRelocDir = pNt->OptionalHeader.DataDirectory;
	pRelocDir = &(pRelocDir[IMAGE_DIRECTORY_ENTRY_BASERELOC]);
	// 4. ��ȡ�ض�λĿ¼
	PIMAGE_BASE_RELOCATION pReloc = (PIMAGE_BASE_RELOCATION)((ULONGLONG)lpImage + pRelocDir->VirtualAddress);
	typedef struct {
		WORD Offset : 12;          // (1) ��СΪ12Bit���ض�λƫ��
		WORD Type : 4;             // (2) ��СΪ4Bit���ض�λ��Ϣ����ֵ
	}TypeOffset, *PTypeOffset;	   // ����ṹ����A1Pass�ܽ��

	// ѭ����ȡÿһ��MAGE_BASE_RELOCATION�ṹ���ض�λ��Ϣ
	while (pReloc->VirtualAddress)
	{
		PTypeOffset pTypeOffset = (PTypeOffset)(pReloc + 1);
		ULONGLONG dwSize = sizeof(IMAGE_BASE_RELOCATION);
		ULONGLONG dwCount = (pReloc->SizeOfBlock - dwSize) / 2;
		for (ULONGLONG i = 0; i < dwCount; i++)
		{
			if (*(PULONGLONG)(&pTypeOffset[i]) == NULL)
				break;
			ULONGLONG dwRVA = pReloc->VirtualAddress + pTypeOffset[i].Offset;
			PULONGLONG pRelocAddr = (PULONGLONG)((ULONGLONG)lpImage + dwRVA);
			// �޸��ض�λ��Ϣ   ��ʽ����Ҫ�޸��ĵ�ַ-ԭӳ���ַ-ԭ���λ�ַ+�����λ�ַ+��ӳ���ַ
			ULONGLONG dwRelocCode = *pRelocAddr - pNt->OptionalHeader.ImageBase - pNt->OptionalHeader.BaseOfCode +
				                                           dwCodeRVA + m_dwImageBase;
			*pRelocAddr = dwRelocCode;
		}
		pReloc = (PIMAGE_BASE_RELOCATION)((ULONGLONG)pReloc + pReloc->SizeOfBlock);
	}
}

void CPE::SetNewOEP(DWORD dwOEP)
{
	m_dwNewOEP = dwOEP;
}

void CPE::ClearBundleImport()
{
	// 1. ��ȡDOSͷ
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)m_pFileBase;
	// 2. ��ȡNTͷ
	PIMAGE_NT_HEADERS64  pNt = (PIMAGE_NT_HEADERS64)((ULONGLONG)m_pFileBase + pDos->e_lfanew);
	// 3. ��ȡ����Ŀ¼��
	PIMAGE_DATA_DIRECTORY pDir = pNt->OptionalHeader.DataDirectory;
	// 4. ��հ��������
	ZeroMemory(&(pDir[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT]), sizeof(IMAGE_DATA_DIRECTORY));
}

void CPE::ClearRandBase()
{
	m_pNT->OptionalHeader.DllCharacteristics = 0;
}

DWORD CPE::GetSectionData(PBYTE lpImage, DWORD dwSectionIndex, PBYTE& lpBuffer, DWORD& dwCodeBaseRVA)
{
	// 1. ��ȡDOSͷ
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)lpImage;
	// 2. ��ȡNTͷ
	PIMAGE_NT_HEADERS64  pNt = (PIMAGE_NT_HEADERS64)((ULONGLONG)lpImage + pDos->e_lfanew);
	// 3. ��ȡ��һ�����νṹ��
	PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);
	DWORD dwSize = pSection[0].SizeOfRawData;

	DWORD dwCodeAddr = (DWORD)lpImage + pSection[0].VirtualAddress;
	lpBuffer = (PBYTE)dwCodeAddr;
	dwCodeBaseRVA = pSection[0].VirtualAddress;
	return dwSize;
}


DWORD CPE::XorCode(BYTE byXOR)
{
	DWORD dwVirtualAddr = m_dwCodeBase;
	DWORD dwOffset = RVA2OffSet(m_dwCodeBase, m_pNT);
	if (!dwOffset)
	{// ���� debug�汾����һ������û�д�С
		PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(m_pNT);
		dwOffset = RVA2OffSet(pSection[1].VirtualAddress, m_pNT);
		dwVirtualAddr = pSection[1].VirtualAddress;
	}
	PBYTE pBase = (PBYTE)((DWORD)m_pFileBase + dwOffset);

	for (DWORD i = 0; i < m_dwCodeSize; i++)
	{
		pBase[i] ^= byXOR;
	}

	return dwVirtualAddr;
}