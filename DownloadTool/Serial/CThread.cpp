#include "../pch.h"
#include "../framework.h"
#include "CThread.h"

CThread::CThread(void)
{
	m_bExit = FALSE;
	m_dwParam = 0;
	m_hThread = NULL;
}

CThread::~CThread(void)
{
	if (!m_bExit) StopThread();
}

// �����������
void CThread::SetThreadData(DWORD dwParam)
{
	if (m_dwParam != dwParam) m_dwParam = dwParam;
}

// ��ȡ����Ĳ���
DWORD CThread::GetThreadData(void)
{
	return m_dwParam;
}

// �̴߳�����
DWORD CThread::ThreadProc(LPVOID lpParam)
{
	CThread *pThis = (CThread *)lpParam;
	if (pThis == NULL)
		return FALSE;

	while (!pThis->m_bExit)
	{
		pThis->runTask();
	}

	return TRUE;
}

// �����������߳�
void CThread::StartThread(void)
{
	
	HANDLE hThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, 
		this, 0, &m_dwThreadId);
	if (hThread == NULL)
		return;
	m_bExit = FALSE; 
	m_hThread = hThread;
}

// �˳��߳�
void CThread::StopThread(void)
{
	if (m_hThread == NULL) return;

	m_bExit = TRUE;
	::WaitForSingleObject(m_hThread, 1000);
	::CloseHandle(m_hThread);
	m_hThread = NULL;
	
}

