// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� SOCKETDLL_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// SOCKETDLL_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef SOCKETDLL_EXPORTS
#define SOCKETDLL_API __declspec(dllexport)
#else
#define SOCKETDLL_API __declspec(dllimport)
#endif

// �����Ǵ� socketdll.dll ������
class SOCKETDLL_API Csocketdll {
public:
	Csocketdll(void);
	// TODO: �ڴ�������ķ�����
};

extern SOCKETDLL_API int nsocketdll;

SOCKETDLL_API int fnsocketdll(void);
class SOCKETDLL_API CInitSocket;