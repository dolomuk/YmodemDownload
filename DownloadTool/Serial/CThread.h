#pragma once
class CThread
{
public:
	CThread(void);
	virtual ~CThread(void);
	virtual void SetThreadData(DWORD dwParam = 0);
	virtual DWORD GetThreadData(void);
	void StartThread(void);
	void StopThread(void);
	// �߳������麯������������
	virtual void runTask(void) = 0;

protected:
	static DWORD ThreadProc(LPVOID lpParam); // �̻߳ص�����

protected:
	HANDLE m_hThread; // �߳̾��
	BOOL m_bExit; // �ж��߳��Ƿ��˳�
	DWORD m_dwParam; // �����ⲿ����ʹ��
	DWORD m_dwThreadId;
};

