#include "../pch.h"
#include "../framework.h"
#include "CSerialPort.h"
#include "../DownloadTool.h"
#include "../DownloadToolDlg.h"
#include "../Ymodem/Ymodem.h"

#define DEBUG_MODE 1

#if DEBUG_MODE
// ��������ڽ�����򿪷�����MessageBox����Ϊ��Ӧ����־�����������
void CSerialPort::ShowError(DWORD dwError) // ��ӡ��ʾ������Ϣ
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dwError,
        0, // Default language
        (LPTSTR)&lpMsgBuf,
        0,
        NULL
    );
    // Display the string.
    MessageBox(NULL, (LPCTSTR)lpMsgBuf, _T("Error"), MB_OK | MB_ICONINFORMATION);
    // Free the buffer.
    LocalFree(lpMsgBuf);
}
#else
void CSerialPort::ShowError(DWORD dwError)
{
}
#endif

CSerialPort::CSerialPort()
{
	m_hComm = NULL;
    memset(&m_dcb, 0, sizeof(m_dcb));
}

CSerialPort::~CSerialPort()
{
    CloseComm();
}

BOOL CSerialPort::isOpen()
{
    if (m_hComm == NULL)
        return FALSE;

    return TRUE;
}

// �򿪴���
BOOL CSerialPort::OpenComm(CString szComm)
{
	if (m_hComm == NULL) {
		m_hComm = ::CreateFile(szComm, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
		if (INVALID_HANDLE_VALUE == m_hComm) {
            ShowError(GetLastError());
            m_hComm = NULL;
            return FALSE;
		}
        return TRUE;
	}
	return FALSE;
}

// ���ô��ڲ���
BOOL CSerialPort::SetCommState(DWORD dwBaudrate, BYTE byParity, BYTE byByteSize, BYTE byStopBits)
{
    if (NULL == m_hComm) return FALSE;
    
    // 1.��ȡ���ڲ���
    BOOL bRet = ::GetCommState(m_hComm, &m_dcb);
    if (!bRet) {
        if (m_hComm) {
            CloseHandle(m_hComm);
            m_hComm = NULL;
        }
        return FALSE;
    }

    // 2.�޸Ĵ��ڲ���
    m_dcb.BaudRate = dwBaudrate;
    m_dcb.ByteSize = byByteSize;
    m_dcb.StopBits = byStopBits;
    m_dcb.Parity = byParity;
    bRet = ::SetCommState(m_hComm, &m_dcb);
    if (!bRet) {
        if (m_hComm) {
            ShowError(GetLastError());
            CloseHandle(m_hComm);
            m_hComm = NULL;
        }
        return FALSE;
    }

    return TRUE;
}

// ���û�������С
BOOL CSerialPort::SetupComm(DWORD dwInQueue, DWORD dwOutQueue)
{
    if (NULL == m_hComm) return FALSE;

    return ::SetupComm(m_hComm, dwInQueue, dwOutQueue);
}

// ������ڱ�־λ
BOOL CSerialPort::PurgeComm(DWORD dwFlags)
{
    if (NULL == m_hComm) return FALSE;

    return ::PurgeComm(m_hComm, dwFlags);
}

// ���ô����¼�����
BOOL CSerialPort::SetCommMask(DWORD dwEvtMask)
{
    if (NULL == m_hComm) return FALSE;

    return ::SetCommMask(m_hComm, dwEvtMask);
}

// д�ļ�����
BOOL CSerialPort::WriteFile(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
{
    if (NULL == m_hComm) return FALSE;

    return ::WriteFile(m_hComm, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}

// ���ļ�����
BOOL CSerialPort::ReadFile(LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped)
{
    if (NULL == m_hComm) return FALSE;

    return ::ReadFile(m_hComm, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}

// �����ص��������
BOOL CSerialPort::GetOverlappedResult(LPOVERLAPPED lpOverlaped, LPDWORD lpNumberOfBytesTransferred, BOOL bWait)
{
    if (NULL == m_hComm) return FALSE;

    return ::GetOverlappedResult(m_hComm, lpOverlaped, lpNumberOfBytesTransferred, bWait);
}

// �������ͨ�Ŵ���
BOOL CSerialPort::ClearCommError(LPDWORD lpErrors, LPCOMSTAT lpStat)
{
    if (NULL == m_hComm) return FALSE;

    return ::ClearCommError(m_hComm, lpErrors, lpStat);
}

// ��ȡ���ڲ���
DCB CSerialPort::GetCommState()
{
    return m_dcb;
}

// ����Ӳ�����֣���������
// �������ڴ��ڴ�֮�󻹿�����
int CSerialPort::SetFlowCtrl()
{
    if (m_hComm != NULL) return FALSE;

    m_dcb.fOutxCtsFlow = TRUE;
    m_dcb.fRtsControl = TRUE;
    return TRUE;
}

// ��ȡ��·״̬
int CSerialPort::GetLineStatus()
{
    DWORD dwEvtmask;
    int ret = 0;
    if (m_hComm == NULL) return FALSE;

    if (GetCommModemStatus(m_hComm, &dwEvtmask) == 0) {
        return FALSE;
    }
    ret = dwEvtmask;
    return ret;
}

// ����DTR��״̬
BOOL CSerialPort::SetDTR(BOOL enable)
{
    if (m_hComm == NULL) return FALSE;

    DWORD data = CLRDTR;
    if (enable) data = SETDTR;
    if (EscapeCommFunction(m_hComm, data)) return TRUE;
    return FALSE;
}

// ����RTS��״̬
BOOL CSerialPort::SetRTS(BOOL enable)
{
    if (m_hComm == NULL) return FALSE;

    DWORD data = CLRRTS;
    if (enable) data = SETRTS;
    if (EscapeCommFunction(m_hComm, data)) return TRUE;
    return FALSE;
}

// �رմ���
BOOL CSerialPort::CloseComm()
{
    // ������Դ
    if (m_hComm == NULL) return FALSE;
    BOOL bRet = CloseHandle(m_hComm);
    m_hComm = NULL;

	// �ر��߳�
	m_ThreadComm.StopThread();

    return bRet;
}

// ���������߳�
void CSerialPort::StartComm(void)
{
    m_ThreadComm.SetThreadData((DWORD)this); // ���봮�����thisָ��
    m_ThreadComm.StartThread(); // �����߳�
}

/*
*********************************************************************************************************
*	�� �� ��: UART_ReceivePacket
*	����˵��: ���շ��Ͷ˷������ַ�
*	��    �Σ�data  ����
*             timeout  ���ʱ��
*	�� �� ֵ: 0 ���ճɹ��� -1 ����ʧ��
*********************************************************************************************************
*/
uint8_t CSerialPort::UART_ReceivePacket(uint8_t* data, uint16_t length, uint32_t timeout)
{
	BOOL  bRead = TRUE;
	BOOL  bResult = TRUE;
	DWORD dwError = 0;
	DWORD BytesRead = 0;
	int cnt = 0;    
    OVERLAPPED m_ov;
    COMSTAT comstat;
	timeout = timeout;

	while (1)
	{
        memset(&comstat, 0, sizeof(COMSTAT));
		bResult = ClearCommError(NULL, &comstat);

		if (comstat.cbInQue == 0)
		{
			Sleep(10);
			if ((timeout--) < 100)
			{
				TRACE(_T("Receive_Byte failed\r\n"));
				return -1;
			}
			continue;
		}

		memset(&m_ov, 0, sizeof(OVERLAPPED));
		bResult = ReadFile(
			data,       /* RX Buffer Pointer */
			length,     /* Read one byte */
			&BytesRead, /* Stores number of bytes read */
			&m_ov);     /* pointer to the m_ov structure */

		PurgeComm(PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
        TRACE("BytesRead=%d,data[0]=0x%02x,data[1]=0x%02x\r\n", BytesRead, data[0], data[1]);
		return 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: Uart_SendByte
*	����˵��: ����һ���ֽ�����
*	��    �Σ�c  �ַ�
*	�� �� ֵ: 0
*********************************************************************************************************
*/
void CSerialPort::UART_SendByte(uint8_t c)
{
	DWORD BytesSent;
    OVERLAPPED m_ov;

    memset(&m_ov, 0, sizeof(OVERLAPPED));
	WriteFile(
		&c, 				/* Pointer to message buffer in calling function */
		1,       		/* Length of message to send */
		&BytesSent,      /* Where to store the number of bytes sent */
		&m_ov);       	/* Overlapped structure */
}

/*
*********************************************************************************************************
*	�� �� ��: UART_SendPacket
*	����˵��: ����һ������
*	��    ��: data  ����
*             length  ���ݳ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void CSerialPort::UART_SendPacket(uint8_t* data, uint16_t length)
{
	DWORD BytesSent;
	OVERLAPPED m_ov;

    memset(&m_ov, 0, sizeof(OVERLAPPED));
	WriteFile(
		data, 		/* Pointer to message buffer in calling function */
		length,       /* Length of message to send */
		&BytesSent,   /* Where to store the number of bytes sent */
		&m_ov);       /* Overlapped structure */
}

/*
*********************************************************************************************************
*	�� �� ��: UART_ReceiveByte
*	����˵��: ���շ��Ͷ˷������ַ�
*	��    �Σ�c  �ַ�
*             timeout  ���ʱ��
*	�� �� ֵ: 0 ���ճɹ��� -1 ����ʧ��
*********************************************************************************************************
*/
uint8_t CSerialPort::UART_ReceiveByte(uint8_t* c, uint32_t timeout)
{
    BOOL  bRead = TRUE;
    BOOL  bResult = TRUE;
    DWORD dwError = 0;
    DWORD BytesRead = 0;
    char RXBuff;
    int cnt = 0;
    OVERLAPPED m_ov;
    COMSTAT comstat;
	CDownloadToolDlg* pDlg = (CDownloadToolDlg*)theApp.GetMainWnd();
	timeout = timeout;

	while (1)
	{
        memset(&comstat, 0, sizeof(COMSTAT));
		bResult =ClearCommError(NULL, &comstat);

		if (comstat.cbInQue == 0)
		{
			Sleep(10);
			if ((timeout--) < 100)
			{
				TRACE(_T("Receive_Byte failed\r\n"));
				return -1;
			}
			continue;
		}

        memset(&m_ov, 0, sizeof(OVERLAPPED));
        m_ov.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		
        bResult = ReadFile(
            &RXBuff,          /* RX Buffer Pointer */
			1,          /* Read one byte */
			&BytesRead, /* Stores number of bytes read */
			&m_ov);     /* pointer to the m_ov structure */

		if (m_ov.hEvent)
			CloseHandle(m_ov.hEvent);

		if (!bResult)
		{
			if (ERROR_IO_PENDING == GetLastError())
			{
                WaitForSingleObject(m_ov.hEvent, 1000);
			}
		}
		PurgeComm(PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
       
        *c = RXBuff;
		TRACE(_T("ReceiveByte = 0x%02x\r\n"), *c);

		return 0;
	}
}
