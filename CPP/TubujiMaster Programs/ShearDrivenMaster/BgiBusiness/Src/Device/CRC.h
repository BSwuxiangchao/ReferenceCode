/*
* @author NanGe 20210929
*/
#ifndef _DEVCOMMON_CRC_H
#define _DEVCOMMON_CRC_H

class CRC
{
public:
	static unsigned short cal_crc(unsigned char* byte, unsigned char nbyte);
	static unsigned short calculateCRC16(unsigned char* frame, unsigned short bufferSize);
};

#endif //_DEVCOMMON_CRC_H
