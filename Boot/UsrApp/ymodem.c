#include "ymodem.h"
#include "crc.h"
#include "usart.h"
#include <stdlib.h>


// ??? ?? ?? ?? ?? ???
pfvCallBack this_cb = NULL;
// ??? ?? ?? ?? ?? ???
pfvSendCallBack pfvSendByte = NULL;

#if defined(USE_USER_STRCONVERT)

#define IS_AF(c)  ((c >= 'A') && (c <= 'F'))
#define IS_af(c)  ((c >= 'a') && (c <= 'f'))
#define IS_09(c)  ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)  IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)  IS_09(c)
#define CONVERTDEC(c)  (c - '0')

#define CONVERTHEX_alpha(c)  (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))

/**
  * @brief  Convert a string to an integer
  * @param  inputstr: The string to be converted
  * @param  intnum: The integer value
  * @retval 1: Correct
  *         0: Error
  */
static uint32_t Str2Int(uint8_t *inputstr, uint32_t *intnum)
{
  uint32_t i = 0, res = 0;
  uint32_t val = 0;

  if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
  {
    if (inputstr[2] == '\0')
    {
      return 0;
    }
    for (i = 2; i < 11; i++)
    {
      if (inputstr[i] == '\0')
      {
        *intnum = val;
        /* return 1; */
        res = 1;
        break;
      }
      if (ISVALIDHEX(inputstr[i]))
      {
        val = (val << 4) + CONVERTHEX(inputstr[i]);
      }
      else
      {
        /* Return 0, Invalid input */
        res = 0;
        break;
      }
    }
    /* Over 8 digit hex --invalid */
    if (i >= 11)
    {
      res = 0;
    }
  }
  else /* max 10-digit decimal input */
  {
    for (i = 0;i < 11;i++)
    {
      if (inputstr[i] == '\0')
      {
        *intnum = val;
        /* return 1 */
        res = 1;
        break;
      }
      else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
      {
        val = val << 10;
        *intnum = val;
        res = 1;
        break;
      }
      else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
      {
        val = val << 20;
        *intnum = val;
        res = 1;
        break;
      }
      else if (ISVALIDDEC(inputstr[i]))
      {
        val = val * 10 + CONVERTDEC(inputstr[i]);
      }
      else
      {
        /* return 0, Invalid input */
        res = 0;
        break;
      }
    }
    /* Over 10 digit decimal --invalid */
    if (i >= 11)
    {
      res = 0;
    }
  }

  return res;
}
#endif
/**
 * @brief  Abort Ymode session
 * @retval None
 */
void Ymodem_Abort(void)
{
	if(pfvSendByte == NULL)
		return;
	pfvSendByte(CA);
	pfvSendByte(CA);
}

/**
 * @brief  Ack received packet
 * @retval None
 */
void Ymodem_SendAck(void)
{
	if(pfvSendByte == NULL)
		return;
	pfvSendByte(ACK);
}

/**
 * @brief  NAk received packet
 * @retval None
 */
void Ymodem_SendNAck(void)
{
	if(pfvSendByte == NULL)
		return;
	pfvSendByte(NAK);
}

/**
 * @brief  start received packet
 * @retval None
 */
void Ymodem_SendCRC16(void)
{
	if(pfvSendByte == NULL)
		return;
	pfvSendByte(CRC16);
}

/**
  * @brief  ?? ??? ?? ? ??? ??? ?? ??
  * @param  [I]func: ?? ?? ??
  * @retval ??
  */
void Ymodem_Init(pfvCallBack func, pfvSendCallBack send_func)
{
	this_cb = func;
	pfvSendByte = send_func;
}

/**
  * @brief  ??? ??? ?? ??
  * @param  [I]buf: ??? ?? ?? ?? ??
  * @param  [I]size: ??? ??
  * @retval ??
  */
static void Handle_DataPackage(uint8_t *buf, uint32_t size)
{
	if(this_cb != NULL)
		this_cb(buf, size);
}

/**
  * @brief  Ymodem ???? ?? ?? ??? ??
  * @param  [I]buf: ???? ?? ?? ??? ?? ?? ??
  * @param  [I]size: ???? ?? ??
  * @param  [O]file_info: ?? ?? ?? ??, @sFileType ??
  * @retval ??
  */
static void Handle_FirstPackage(uint8_t *buf, uint32_t size, sFileType *file_info)
{
	uint16_t i = 0, index = 0;
	char size_buf[FILE_SIZE_LENGTH] = {0};
	// char modify_time_buf[FILE_MODIFYSEC_LENGTH] = {0}; // ?? ?? ??
	// ?? ?? ??
	while(buf[i] != 0x00 && i < size)
	{
		file_info->file_name[index++] = buf[i];
		i++;
	}
	// ?? ?? ??
	char *p_type = strstr((const char *)(file_info->file_name), ".");
	if(NULL != p_type)
	{
		if(strcmp(".bin", p_type) == 0)
			file_info->file_type = BINARY_FILE;
		else if(strcmp(".hex", p_type) == 0)
			file_info->file_type = HEX_FILE;
		else if(strcmp(".s19", p_type) == 0)
			file_info->file_type = S19_FILE;
		else
			file_info->file_type = UNKONW_FILE;
	}
	// ?? ?? ??, ??: ???
	i++;
	index = 0;
	while((buf[i] != 0x20 && buf[i] != 0x00) && i < size)
	{
		size_buf[index++] = buf[i];
		i++;
	}	
#if defined(USE_USER_STRCONVERT)
	Str2Int((uint8_t *)size_buf, &(file_info->file_size));
#else
	file_info->file_size = atol(size_buf);
#endif
}

/**
  * @brief  Ymodem ???? ??? ?? ??
  * @param  [I]buf: ???? ??? ?? ??? ?? ?? ??
  * @param  [I]size: ???? ?? ??
  * @param  [O]file_info: ?? ?? ?? ??, @sFileType ??
  * @retval ??
  */
static void Handle_PackageData(uint8_t *buf, uint32_t size, uint32_t index, sFileType *file_info)
{
	if(file_info == NULL)
		return;

	if(index == PACKET_FILEINFO_INDEX) // ?? ??
	{
		Handle_FirstPackage(buf, size, file_info);
		Ymodem_SendAck();
		Ymodem_SendCRC16();
	}
	else
	{
		Handle_DataPackage(buf, size);
		Ymodem_SendAck();
	}
}

/**
  * @brief  ?? ?? ?? ??
  * @param  [O]isEndFlag: 2?? 0x04 ?? ?? ?, 1? ??
  * @retval ??
  */
static void EndCodeAck_Handler(uint8_t *isEndFlag)
{
	static uint8_t eot_count = 0; // ?? ?? ???

	(*isEndFlag)++;
	if(eot_count > 0) // 2?? 0x04 ??
	{
		Ymodem_SendAck();
		Ymodem_SendCRC16();
		return;
	}
	// 1?? 0x04 ??
	Ymodem_SendNAck();
	eot_count++;
}

/**
  * @brief  Ymodem ?? ??
  * @param  [I]buf: ???? ??? ?? ??? ?? ?? ??
  * @param  [I]size: ???? ?? ??
  * @param  [O]file_info: ?? ?? ?? ??, @sFileType ??
  * @param  [O]recv_size: ?? ??? ?? ?? ?? (?? ?? ? ?? ?? ?? ??)
  * @retval ?? ?? ??
  */
eYmodemStatus Ymodem_Receive(uint8_t *buf, uint32_t buf_size, sFileType *file_info, uint32_t * recv_size)
{
	static uint32_t package_count 	= 0; 	//	?? ???
	static uint32_t error_count 	= 0; 	//	?? ???
	static uint8_t isEndFlag 		= 0; 	//	?? ??? ???

	uint32_t package_size; 								//	?? ??
	uint16_t crc_16 = 0;   								//	CRC16 ??
	uint8_t package_type = buf[PACKET_FILEINFO_INDEX];	//	?? ??

	eYmodemStatus status = YMODEM_NO_FILE; //	?? ??
	
	if(buf == NULL)
	{
		status = YMODEM_ABORTED;
		goto RETURN;
	}
	
	if(package_count != 0 && (buf_size == 1) && (buf[0] == EOT)) // ?? ?? ??
	{
		EndCodeAck_Handler(&isEndFlag);
		if(isEndFlag)
		{
			status = YMODEM_CONTINUE;
			goto RETURN;
		}
	}
	
	// ?? ?? ??? ??
	if(buf_size > (PACKET_1K_SIZE + PACKET_OVERHEAD) || buf_size < (PACKET_SIZE + PACKET_OVERHEAD))
	{
		Ymodem_Abort();
		status = YMODEM_ABORTED;
		goto RETURN;
	}

	// ?? ?? ??
	if((buf[PACKET_SEQNO_INDEX] + buf[PACKET_SEQNO_COMP_INDEX]) != 0xFF)
	{
		Ymodem_Abort();
		status = YMODEM_ABORTED;
		goto RETURN;
	}
	
	package_size = (package_type == SOH) ? PACKET_SIZE : PACKET_1K_SIZE;

	// CRC16 ??
	crc_16 = crc16(buf + PACKET_HEADER, package_size);

	if( crc_16 != ((buf[buf_size - 2] << 8) | buf[buf_size - 1]))
	{
		status = YMODEM_CONTINUE;
		if(error_count++ > MAX_ERRORS)
		{
			status = YMODEM_TOO_MANY_ERRORS;
			Ymodem_Abort();
			goto RETURN;
		}
		Ymodem_SendNAck(); // ??? ??
	}
	
	if(isEndFlag) // ?? ???
	{
		package_count 	=	0;
		isEndFlag 		=	0;
		error_count 	=	0;
		
		if(buf[PACKET_HEADER] == 0x00)
		{
			Ymodem_SendAck();
		#ifdef SUPPORT_CONTINUE_DOWNLOAD_MODE // ?? ??
			Ymodem_SendCRC16();
			status = YMODEM_CONTINUE;
		#else
			status = YMODEM_DONE;
		#endif
			goto RETURN;
		}
	}
	
	Handle_PackageData(buf + PACKET_HEADER, package_size, package_count, file_info);

	if(package_count == 0)
	{
		status = YMODEM_FILE_INFO_RECV;
	}
	else
	{
		(*recv_size) += package_size; // ?? ?? ??
		status = YMODEM_CONTINUE;
	}
	
	package_count++;

RETURN:
	return status;
}
