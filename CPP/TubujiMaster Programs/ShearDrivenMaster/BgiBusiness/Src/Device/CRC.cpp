
#include "CRC.h"

unsigned short CRC::cal_crc(unsigned char* byte, unsigned char nbyte)
{
	unsigned short itemp = 0xFFFF;
	unsigned char i;
	while (nbyte--)
	{
		itemp ^= *byte;
		byte++;
		for (i = 0; i < 8; i++)
		{
			if (itemp & 0x1)
			{
				itemp >>= 1;
				itemp ^= 0x1A001;
			}
			else
			{
				itemp >>= 1;
			}
		}
	}
	return itemp;
}

unsigned short CRC::calculateCRC16(unsigned char* frame, unsigned short bufferSize)
{
	unsigned short temp, temp2, flag;
	temp = 0xFFFF;
	for (unsigned char i = 0; i < bufferSize; i++)
	{
		temp = temp ^ frame[i];
		for (unsigned char j = 1; j <= 8; j++)
		{
			flag = temp & 0x0001;
			temp >>= 1;
			if (flag)
				temp ^= 0xA001;
		}
	}
	// Reverse byte order. 
	temp2 = temp >> 8;
	temp = (temp << 8) | temp2;
	temp &= 0xFFFF;
	return temp;
}


