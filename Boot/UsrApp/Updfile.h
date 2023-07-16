#ifndef __UPDFILE_H__
#define __UPDFILE_H__

#include <stdint.h>

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


#endif /* __UPDFILE_H__ */
