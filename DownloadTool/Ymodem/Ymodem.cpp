
#include "../pch.h"
#include "../Ymodem/Ymodem.h"
#include "../Serial/CSerialPort.h"

/*
*********************************************************************************************************
*	                                   Ymodem�ļ�����Э�����
*********************************************************************************************************
*/
/*
��1�׶Σ� ͬ��
	�ӻ������ݷ���ͬ���ַ� C

��2�׶Σ����͵�1֡���ݣ������ļ������ļ���С
	�������ͣ�
	---------------------------------------------------------------------------------------
	| SOH  |  ��� - 0x00 |  ���ȡ�� - 0xff | 128�ֽ����ݣ����ļ������ļ���С�ַ���|CRC0 CRC1|
	|-------------------------------------------------------------------------------------|
	�ӻ����գ�
	���ճɹ��ظ�ACK��CRC16������ʧ�ܣ�У�����������󣩼����ظ��ַ�C������һ������������ظ�����CA����ֹ���䡣

��3�׶Σ����ݴ���
	�������ͣ�
	---------------------------------------------------------------------------------------
	| SOH/STX  |  ��0x01��ʼ���  |  ���ȡ�� | 128�ֽڻ���1024�ֽ�                |CRC0 CRC1|
	|-------------------------------------------------------------------------------------|
	�ӻ����գ�
	���ճɹ��ظ�ACK������ʧ�ܣ�У�����������󣩻����û�����ʧ�ܼ����ظ��ַ�C������һ������������ظ�����CA����ֹ���䡣

��4�׶Σ�����֡
	�������ͣ�����EOT�������䡣
	�ӻ����գ��ظ�ACK��

��5�׶Σ���֡������ͨ��
	�������ͣ�һ֡�����ݡ�
	�ӻ����գ��ظ�ACK��
*/


/*
*********************************************************************************************************
*	�� �� ��: Int2Str
*	����˵��: ������ת�����ַ�
*	��    ��: str �ַ�  intnum ����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Int2Str(uint8_t* str, int32_t intnum)
{
	uint32_t i, Div = 1000000000, j = 0, Status = 0;

	for (i = 0; i < 10; i++)
	{
		str[j++] = (intnum / Div) + 48;

		intnum = intnum % Div;
		Div /= 10;
		if ((str[j - 1] == '0') && (Status == 0))
		{
			j = 0;
		}
		else
		{
			Status++;
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: Ymodem_PrepareIntialPacket
*	����˵��: ׼����һ��Ҫ���͵�����
*	��    ��: data ����
*             fileName �ļ���
*             length   �ļ���С
*	�� �� ֵ: 0
*********************************************************************************************************
*/
void Ymodem_PrepareIntialPacket(uint8_t* data, const uint8_t* fileName, uint32_t* length)
{
	uint16_t i, j;
	uint8_t file_ptr[10];

	/* ��һ�����ݵ�ǰ�����ַ�  */
	data[0] = SOH; /* soh��ʾ���ݰ���128�ֽ� */
	data[1] = 0x00;
	data[2] = 0xff;

	/* �ļ��� */
	for (i = 0; (fileName[i] != '\0') && (i < FILE_NAME_LENGTH); i++)
	{
		data[i + PACKET_HEADER] = fileName[i];
	}

	data[i + PACKET_HEADER] = 0x00;

	/* �ļ���Сת�����ַ� */
	_itot_s(*length, (TCHAR *)file_ptr, sizeof(file_ptr), 10);

	for (j = 0, i = i + PACKET_HEADER + 1; file_ptr[j] != '\0'; )
	{
		data[i++] = file_ptr[j++];
	}
	data[i + PACKET_HEADER] = 0x00;

	/* ���ಹ0 */
	for (j = i; j < PACKET_SIZE + PACKET_OVERHEAD; j++)
	{
		data[j] = 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: Ymodem_PreparePacket
*	����˵��: ׼���������ݰ�
*	��    ��: SourceBuf Ҫ���͵�ԭ����
*             data      ����Ҫ���͵����ݰ����Ѿ�������ͷ�ļ���ԭ����
*             pktNo     ���ݰ����
*             sizeBlk   Ҫ����������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int sendsize = 0;
void Ymodem_PreparePacket(uint8_t* SourceBuf, uint8_t* data, uint8_t pktNo, uint32_t sizeBlk)
{
	uint16_t i, size, packetSize;
	uint8_t* file_ptr;

	/* ���ú�Ҫ�������ݰ���ǰ�����ַ�data[0]��data[1]��data[2] */
	/* ����sizeBlk�Ĵ�С�������������ݸ�����ȡ1024�ֽڻ���ȡ128�ֽ�*/
	packetSize = sizeBlk >= PACKET_1K_SIZE ? PACKET_1K_SIZE : PACKET_SIZE;

	/* ���ݴ�С��һ��ȷ�� */
	size = sizeBlk < packetSize ? sizeBlk : packetSize;

	/* ���ֽڣ�ȷ����1024�ֽڻ�����128�ֽ� */
	if (packetSize == PACKET_1K_SIZE)
	{
		data[0] = STX;
	}
	else
	{
		data[0] = SOH;
	}

	/* ��2���ֽڣ�������� */
	data[1] = pktNo;
	/* ��3���ֽڣ��������ȡ�� */
	data[2] = (~pktNo);
	file_ptr = SourceBuf;

	/* ���Ҫ���͵�ԭʼ���� */
	for (i = PACKET_HEADER; i < size + PACKET_HEADER; i++)
	{
		data[i] = *file_ptr++;
	}

	/* ����Ĳ� EOF (0x1A) �� 0x00 */
	if (size <= packetSize)
	{
		for (i = size + PACKET_HEADER; i < packetSize + PACKET_HEADER; i++)
		{
			data[i] = 0x1A; /* EOF (0x1A) or 0x00 */
		}
	}

	sendsize += size;

	TRACE(_T("SendSize = %d\r\n"), sendsize);
	::PostMessage((HWND)theApp.GetMainWnd()->m_hWnd, WM_WRITE_PACK_FINISH_MSG, (WPARAM)WM_WRITE_PACK_FINISH_MSG, (LPARAM)&sendsize);
}

/*
*********************************************************************************************************
*	�� �� ��: UpdateCRC16
*	����˵��: �ϴμ����CRC��� crcIn �ټ���һ���ֽ����ݼ���CRC
*	��    ��: crcIn ��һ��CRC������
*             byte  ������ֽ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint16_t UpdateCRC16(uint16_t crcIn, uint8_t byte)
{
	uint32_t crc = crcIn;
	uint32_t in = byte | 0x100;

	do
	{
		crc <<= 1;
		in <<= 1;
		if (in & 0x100)
			++crc;
		if (crc & 0x10000)
			crc ^= 0x1021;
	} while (!(in & 0x10000));

	return crc & 0xffffu;
}

/*
*********************************************************************************************************
*	�� �� ��: Cal_CRC16
*	����˵��: ����һ�����ݵ�CRC
*	��    ��: data  ����
*             size  ���ݳ���
*	�� �� ֵ: CRC������
*********************************************************************************************************
*/
uint16_t Cal_CRC16(const uint8_t* data, uint32_t size)
{
	uint32_t crc = 0;
	const uint8_t* dataEnd = data + size;

	while (data < dataEnd)
		crc = UpdateCRC16(crc, *data++);

	crc = UpdateCRC16(crc, 0);
	crc = UpdateCRC16(crc, 0);

	return crc & 0xffffu;
}

/*
*********************************************************************************************************
*	�� �� ��: CalChecksum
*	����˵��: ����һ�������ܺ�
*	��    ��: data  ����
*             size  ���ݳ���
*	�� �� ֵ: �������ĺ�8λ
*********************************************************************************************************
*/
uint8_t CalChecksum(const uint8_t* data, uint32_t size)
{
	uint32_t sum = 0;
	const uint8_t* dataEnd = data + size;

	while (data < dataEnd)
		sum += *data++;

	return (sum & 0xffu);
}

/*
*********************************************************************************************************
*	�� �� ��: Ymodem_Transmit
*	����˵��: �����ļ�
*	��    ��: buf  �ļ�����
*             sendFileName  �ļ���
*             sizeFile    �ļ���С
*	�� �� ֵ: 0  �ļ����ͳɹ�
*********************************************************************************************************
*/
uint8_t Ymodem_Transmit(uint8_t* buf, const uint8_t* sendFileName, uint32_t sizeFile)
{
	uint8_t packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD] = { 0 };
	uint8_t filename[FILE_NAME_LENGTH];
	uint8_t* buf_ptr, tempCheckSum;
	uint16_t tempCRC;
	uint16_t blkNumber;
	uint8_t receivedC[64] = {0}, CRC16_F = 0, i;
	uint32_t errors, ackReceived, size = 0, pktSize;

	CDownloadToolDlg* pDlg = (CDownloadToolDlg*)theApp.GetMainWnd();
	CSerialPort *pCom = (CSerialPort *)pDlg->OnGetCommPortHandle();
	errors = 0;
	ackReceived = 0;
	for (i = 0; i < (FILE_NAME_LENGTH - 1); i++)
	{
		filename[i] = sendFileName[i];
	}

	CRC16_F = 1;

	/* ��ʼ��Ҫ���͵ĵ�һ�����ݰ� */
	Ymodem_PrepareIntialPacket(&packet_data[0], filename, &sizeFile);

#if 0
	/* �ȴ���һ�����ݰ��ɷ��� */
	do
	{
		if (pCom->UART_ReceivePacket(&receivedC_start, 1, 10000) == 0)
		{
			if ((receivedC_start == CRC16))
			{
				/* ���յ�Ӧ�� */
				ackReceived = 1;
			}
		}
		/* û�еȵ� */
		else
		{
			errors++;
		}
	} while (!ackReceived && (errors < 0x0A));
#else
	ackReceived = 1;
#endif
	if (ackReceived)
	{
		ackReceived = 0;
		do
		{
			/* ����CRC16_F����CRC������ͽ���У�� */
			if (CRC16_F)
			{
				tempCRC = Cal_CRC16(&packet_data[PACKET_HEADER], PACKET_SIZE);
				packet_data[PACKET_HEADER + PACKET_SIZE] = (tempCRC >> 8);
				packet_data[PACKET_HEADER + PACKET_SIZE+1] = (tempCRC & 0xFF);
				pCom->UART_SendPacket(packet_data, PACKET_SIZE + PACKET_OVERHEAD);
			}
			else
			{
				tempCheckSum = CalChecksum(&packet_data[3], PACKET_SIZE);
				pCom->UART_SendByte(tempCheckSum);
			}

			/* �ȴ� Ack ���ַ� 'C' */
			if (pCom->UART_ReceivePacket(&receivedC[0], 2, 1000) == 0)
			{
				if ((receivedC[0] == ACK) && (receivedC[1] == CRC16))
				{
					/* ���յ�Ӧ�� */
					ackReceived = 1;
				}
			}
			/* û�еȵ� */
			else
			{
				errors++;
			}
			/* �������ݰ�����յ�Ӧ�����û�еȵ����Ƴ� */
		} while (!ackReceived && (errors < 0x0A));
	}

	/* ����������������˳� */
	if (errors >= 0x0A)
	{
		return errors;
	}

	buf_ptr = buf;
	size = sizeFile;
	blkNumber = 0x01;

	/* ����ʹ�õ��Ƿ���1024�ֽ����ݰ� */
	/* Resend packet if NAK  for a count of 10 else end of communication */
	while (size)
	{
		/* ׼����һ������ */
		Ymodem_PreparePacket(buf_ptr, &packet_data[0], (uint8_t)blkNumber, size);
		ackReceived = 0;
		receivedC[0] = 0;
		errors = 0;
		do
		{
			/* ������һ������ */
			if (size >= PACKET_1K_SIZE)
			{
				pktSize = PACKET_1K_SIZE;
			}
			else
			{
				pktSize = PACKET_SIZE;
			}

			/* ����CRC16_F����CRCУ�������͵Ľ�� */
			if (CRC16_F)
			{
				tempCRC = Cal_CRC16(&packet_data[PACKET_HEADER], pktSize);
				packet_data[PACKET_HEADER + pktSize] = (tempCRC >> 8);
				packet_data[PACKET_HEADER + pktSize + 1] = (tempCRC & 0xFF);
				pCom->UART_SendPacket(packet_data, pktSize + PACKET_OVERHEAD);
			}
			else
			{
				tempCheckSum = CalChecksum(&packet_data[3], pktSize);
				pCom->UART_SendByte(tempCheckSum);
			}

			/* �ȵ�Ack�ź� */
			if ((pCom->UART_ReceiveByte(&receivedC[0], 1000) == 0) && (receivedC[0] == ACK))
			{
				ackReceived = 1;
				/* �޸�buf_ptrλ���Լ�size��С��׼��������һ������ */
				if (size > pktSize)
				{
					buf_ptr += pktSize;
					size -= pktSize;
					if (blkNumber == ((2 * 1024 * 1024) / 128))
					{
						return 0xFF; /* ���� */
					}
					else
					{
						blkNumber++;
					}
				}
				else
				{
					buf_ptr += pktSize;
					size = 0;
				}
			}
			else
			{
				errors++;
			}

		} while (!ackReceived && (errors < 0x0A));

		/* ����10��û���յ�Ӧ����˳� */
		if (errors >= 0x0A)
		{
			return errors;
		}
	}


	ackReceived = 0;
	receivedC[0] = 0x00;
	errors = 0;
	do
	{
		/* ����EOT�ź� */
		pCom->UART_SendByte(EOT);

		/* �ȴ�AckӦ�� */
		if (!ackReceived)
		{
			if ((pCom->UART_ReceiveByte(&receivedC[0], 1000) == 0))
			{
				if (receivedC[0] == NAK)
				{
					ackReceived++;
				}
			}
			else
			{
				errors++;
			}
		}
		else
		{
			if ((pCom->UART_ReceivePacket(receivedC, 2, 1000) == 0))
			{
				if (receivedC[0] == ACK && receivedC[1] == CRC16)
				{
					ackReceived++;
				}
			}
			else
			{
				errors++;
			}
		}

	} while ((ackReceived<2) && (errors < 0x0A));


	if (errors >= 0x0A)
	{
		return errors;
	}

	TRACE(_T("���ͽ����ź�\r\n"));

#if 1
	/* ��ʼ�����һ��Ҫ���͵����� */
	ackReceived = 0;
	receivedC[0] = 0x00;
	errors = 0;

	packet_data[0] = SOH;
	packet_data[1] = 0;
	packet_data[2] = 0xFF;

	/* ���ݰ������ݲ���ȫ����ʼ��Ϊ0 */
	for (i = PACKET_HEADER; i < (PACKET_SIZE + PACKET_HEADER); i++)
	{
		packet_data[i] = 0x00;
	}

	do
	{
		/* �������ݰ� */
		/* ����CRC16_F����CRCУ�������͵Ľ�� */
		tempCRC = Cal_CRC16(&packet_data[PACKET_HEADER], PACKET_SIZE);
		packet_data[PACKET_HEADER + PACKET_SIZE] = (tempCRC >> 8);
		packet_data[PACKET_HEADER + PACKET_SIZE + 1] = (tempCRC & 0xFF);
		pCom->UART_SendPacket(packet_data, PACKET_SIZE + PACKET_OVERHEAD);

		/* �ȴ� Ack */
		if (pCom->UART_ReceiveByte(&receivedC[0], 1000) == 0)
		{
			if (receivedC[0] == ACK)
			{
				/* ���ݰ����ͳɹ� */
				ackReceived = 1;
			}
		}
		else
		{
			errors++;
		}
	} while (!ackReceived && (errors < 0x0A));

	TRACE(_T("�������\r\n"));

	/* ����10��û���յ�Ӧ����˳� */
	if (errors >= 0x0A)
	{
		return errors;
	}
#endif

	return 0; /* �ļ����ͳɹ� */
}

/*
*********************************************************************************************************
*	�� �� ��: xymodem_send
*	����˵��: �����ļ�
*	��    ��: filename  �ļ���
*	�� �� ֵ: 0  �ļ����ͳɹ�
*********************************************************************************************************
*/
int xymodem_send(sFileType *file, uint8_t *buf)
{
	sendsize = 0;
	Ymodem_Transmit(buf, file->file_name, file->file_size);
	return 0;
}
