// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� PACK_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// PACK_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef PACK_EXPORTS
#define PACK_API __declspec(dllexport)
#else
#define PACK_API __declspec(dllimport)
#endif

// �����Ǵ� Pack.dll ������
class PACK_API CPack {
public:
	CPack(void);
	// TODO:  �ڴ�������ķ�����
};

extern PACK_API int nPack;

PACK_API int fnPack(void);

extern "C" PACK_API BOOL Pack(CString strPath, BYTE byXor);