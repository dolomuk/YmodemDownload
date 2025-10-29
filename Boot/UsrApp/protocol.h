#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

/* ?? ???? ?? */
#define YMODEM	(0x01)
#define XMODEM	(0x02)
#define ZMODEM	(0x03)

#define PROTOCOL_TYPE	YMODEM

#define BOOT_CMD_HAND_CODE 				0xA5	 // ????? ??
#define BOOT_CMD_READ_STARTINFO         0X10     // ?? ?? ??
#define BOOT_CMD_READ_BOOTSTATE         0X17     // ?? ?? ??
#define BOOT_CMD_READ_IAP_RDDATA        0X18     // APP ?? ??? ??
#define BOOT_CMD_READ_SYSTICK           0X19     // ??? TICK ??

#define BOOT_CMD_WRITE_IAP_CTRL         0X87     // ????? ??
#define BOOT_CMD_WRITE_IAP_WRDATA       0X88     // APP ?? ??? ??
#define BOOT_CMD_WRITE_IAP_RES          0X89     // ????? ??

#define SOF


#endif /* __PROTOCOL_H__ */
