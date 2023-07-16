#pragma once

// Э��ͷ
#define HEARD_ID		0xAA	// Э��ͷ

// Э���ʶ
#define SEND_ID			0x9F	// ��λ�����ͱ�ʶ
#define DEBUG_ID		0xF1	// ������������ʶ
#define CHANG_BAUND_ID	0x88	// ���Ĵ��ڲ�����Э�飬������Ч

// Ӧ���ʶ
#define CMD_ACK_ID		0x80	// Ӧ�������ʶ
#define DATA_ACK_ID		0x01	// ��λ������Ӧ���ʶ
#define	HAND_CODE		0x80	// ������
#define	OUTPUT_CMD		0x7F	// ���ò����������
#define	COOLECT_CMD		0x7E	// ��λ���ɼ�����
#define GET_OUTPUTWAVE	0x79	// ��ȡ���������һ����������	

#define SETBAUND_CMD	0x00	// ���ò�����
#define READBAUND_CMD	0x01	// ��ȡ������

// ����ͨѶ�ӿڷ�ʽ
typedef enum
{
	UART_MODE = 0,
	I2C_MODE,
	SPI_MODE,
	GPIO_MODE,
} eDownLoadMode;

// �������б�
typedef enum
{
	BAUD_1200_RATE    = 1200,
	BAUD_2400_RATE    = 2400,
	BAUD_4800_RATE    = 4800,
	BAUD_9600_RATE    = 9600,
	BAUD_19200_RATE   = 19200,
	BAUD_38400_RATE   = 38400,
	BAUD_115200_RATE  = 115200,
	BAUD_230400_RATE  = 230400,
	BAUD_393939_RATE  = 393939,
	BAUD_460800_RATE  = 460800,
	BAUD_921600_RATE  = 921600,
	BAUD_2000000_RATE = 2000000,
	BAUD_3000000_RATE = 3000000,
} eBaudRateList;

typedef enum
{
	BAUD_1200_INDEX,
	BAUD_2400_INDEX,
	BAUD_4800_INDEX,
	BAUD_9600_INDEX,
	BAUD_19200_INDEX,
	BAUD_38400_INDEX,
	BAUD_115200_INDEX,
	BAUD_230400_INDEX,
	BAUD_393939_INDEX,
	BAUD_460800_INDEX,
	BAUD_921600_INDEX,
	BAUD_2000000_INDEX,
	BAUD_3000000_INDEX,
} eBaudRateIndex;

// �����ļ�����
#define MAX_FILENAME_SIZE	256
typedef enum
{
	UNKONW_FILE, // δ֪
	BINARY_FILE, // bin��ʽ�ļ�
	HEX_FILE, // hex��ʽ�ļ�
	S19_FILE, // s19��ʽ�ļ�
} eFileType;

typedef struct
{
	uint8_t file_name[MAX_FILENAME_SIZE]; // �ļ���
	eFileType file_type; // �ļ�����
	uint32_t file_size; // �ļ���С
	uint32_t tm_modify; // �޸�ʱ����
} sFileType;
