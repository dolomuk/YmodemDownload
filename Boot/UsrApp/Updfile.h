#ifndef __UPDFILE_H__
#define __UPDFILE_H__

#include <stdint.h>

#define MAX_FILENAME_SIZE	256

typedef enum
{
	UNKONW_FILE, // �� �� ����
	BINARY_FILE, // bin ���� ����
	HEX_FILE, // hex ���� ����
	S19_FILE, // s19 ���� ����
} eFileType;

typedef struct 
{
	uint8_t file_name[MAX_FILENAME_SIZE]; // ���� �̸�
	eFileType file_type; // ���� ����
	uint32_t file_size; // ���� ũ��
	uint32_t tm_modify; // ���� �ð�(��)
} sFileType;


#endif /* __UPDFILE_H__ */
