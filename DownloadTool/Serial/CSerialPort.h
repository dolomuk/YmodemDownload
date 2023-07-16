#pragma once

#include "CThreadComm.h"

// ��������С
#define TX_BUFSIZE (4*1024)
#define RX_BUFSIZE (4*1024)

class CSerialPort
{
	HANDLE m_hComm;
	DCB m_dcb;
	CThreadComm m_ThreadComm; // �����߳�ʹ�ã���дrunTask����ʵ�ִ���������

	void ShowError(DWORD dwError);
protected:
	
public:
	CSerialPort();
	~CSerialPort(); // ��������

	BOOL isOpen();
	// �򿪴���
	BOOL OpenComm(CString szComm); 
	// ���ô��ڲ���
	BOOL SetCommState(DWORD dwBaudrate, BYTE byParity, BYTE byByteSize, BYTE byStopBits); 
	// ���÷��ͺͽ��ջ�������С
	BOOL SetupComm(DWORD dwInQueue, DWORD dwOutQueue); 
	// ������ڱ�־
	BOOL PurgeComm(DWORD dwFlags); 
	// ���ô����¼�����
	BOOL SetCommMask(DWORD dwEvtMask); 
	// д�ļ�����
	BOOL WriteFile(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped);
	// ���ļ�����
	BOOL ReadFile(LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped); 
	// ��ȡ�ص�����״̬
	BOOL GetOverlappedResult(LPOVERLAPPED lpOverlaped, LPDWORD lpNumberOfBytesTransferred, BOOL bWait);
	// �������ͨ�Ŵ���
	BOOL ClearCommError(LPDWORD lpErrors, LPCOMSTAT lpStat);
	// ��ȡ���ڲ���
	DCB GetCommState(void);
	// ��������
	BOOL SetFlowCtrl(void);
	// ��ȡ��·״̬
	int GetLineStatus(void);
	// ����DTR״̬
	BOOL SetDTR(BOOL enable);
	// ����RTS״̬
	BOOL SetRTS(BOOL enable);
	// �رմ���
	BOOL CloseComm(void);

public:
	// �������ڽ����߳�
	void StartComm(void);

	uint8_t UART_ReceivePacket(uint8_t* data, uint16_t length, uint32_t timeout);
	void UART_SendByte(uint8_t c);
	void UART_SendPacket(uint8_t* data, uint16_t length);
	uint8_t UART_ReceiveByte(uint8_t* c, uint32_t timeout);
};

