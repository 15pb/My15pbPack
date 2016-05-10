// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� STUB_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// STUB_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef STUB_EXPORTS
#define STUB_API __declspec(dllexport)
#else
#define STUB_API __declspec(dllimport)
#endif

extern "C"
{
	typedef struct _GLOBAL_PARAM
	{
		DWORD       dwStart;               // Stub�е�OEP
		BOOL        bShowMessage;          // �Ƿ���ʾ������Ϣ
		ULONGLONG   dwImageBase;           // ӳ���ַ
		DWORD       dwOEP;		           // ������ڵ�
		PBYTE       lpStartVA;	           // ��ʼ�����ַ��������������
		DWORD       dwCodeSize;	           // �����ܴ�С
		BYTE        byXor;                 // ��������ֵ
	}GLOBAL_PARAM, *PGLOBAL_PARAM;

	typedef DWORD(WINAPI *fnGetProcAddress)(_In_ HMODULE hModule, _In_ LPCSTR lpProcName);
	typedef HMODULE(WINAPI *fnLoadLibraryA)(_In_ LPCSTR lpLibFileName);
	typedef int(WINAPI *fnMessageBox)(HWND hWnd, LPSTR lpText, LPSTR lpCaption, UINT uType);
	typedef HMODULE(WINAPI *fnGetModuleHandleA)(_In_opt_ LPCSTR lpModuleName);
	typedef BOOL(WINAPI *fnVirtualProtect)(_In_ LPVOID lpAddress, _In_ SIZE_T dwSize, _In_ DWORD flNewProtect, _Out_ PDWORD lpflOldProtect);
	typedef void(WINAPI *fnExitProcess)(_In_ UINT uExitCode);

	void Start();
}




