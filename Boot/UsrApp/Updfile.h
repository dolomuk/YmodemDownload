#ifndef __UPDFILE_H__
#define __UPDFILE_H__

#include <stdint.h>

#define MAX_FILENAME_SIZE	256

typedef enum
{
	UNKONW_FILE, // 알 수 없음
	BINARY_FILE, // bin 형식 파일
	HEX_FILE, // hex 형식 파일
	S19_FILE, // s19 형식 파일
} eFileType;

typedef struct 
{
	uint8_t file_name[MAX_FILENAME_SIZE]; // 파일 이름
	eFileType file_type; // 파일 유형
	uint32_t file_size; // 파일 크기
	uint32_t tm_modify; // 수정 시간(초)
} sFileType;


#endif /* __UPDFILE_H__ */
