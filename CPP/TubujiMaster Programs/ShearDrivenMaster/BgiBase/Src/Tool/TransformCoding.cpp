#include "TransformCoding.h"

QString StringConversionEncoding(const char* data, FileCodeing type)
{
	QString str = QString::fromLocal8Bit("中_文_乱_码");
	if (NULL == data)
	{
		return str;
	}
	if (IsUTF8(data, strlen(data)))
	{
		type = FILE_CODEING_UTF_8;
	}
	else
	{
		type = FILE_CODEING_GB2312;
	}
	switch (type)
	{
	case FileCodeing::FILE_CODEING_GB2312:
		str = QString::fromLocal8Bit(data);
		break;
	case FileCodeing::FILE_CODEING_UTF_8:
		str = QString::fromUtf8(data);
		break;
	default:
		break;
	}
	return str;
}

QString BGIBASE_CAPI_EXPORT StringConversionEncoding(const char* data)
{
	QString str = QString::fromLocal8Bit("中_文_乱_码");
	if (NULL == data)
	{
		return str;
	}
	if (IsUTF8(data, strlen(data)))
	{
		str = QString::fromUtf8(data);
	}
	else
	{
		str = QString::fromLocal8Bit(data);
	}
	return str;
}


int preNUm(char byte) {
	unsigned char mask = 0x80;
	int num = 0;
	for (int i = 0; i < 8; i++) {
		if ((byte & mask) == mask) {
			mask = mask >> 1;
			num++;
		}
		else {
			break;
		}
	}
	return num;
}

bool IsUTF8(const char* data, int len)
{
	int num = 0;
	int i = 0;
	while (i < len) {
		if ((data[i] & 0x80) == 0x00) {
			// 0XXX_XXXX
			i++;
			continue;
		}
		else if ((num = preNUm(data[i])) > 2) {
			// 110X_XXXX 10XX_XXXX
			// 1110_XXXX 10XX_XXXX 10XX_XXXX
			// 1111_0XXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
			// 1111_10XX 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
			// 1111_110X 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
			// preNUm() 返回首个字节8个bits中首个bit前面1bit的个数，该数量也是该字符所使用的字节数        
			i++;
			for (int j = 0; j < num - 1; j++) {
				//判断后面num - 1 个字节是不是都是10开
				if ((data[i] & 0xc0) != 0x80) {
					return false;
				}
				i++;
			}
		}
		else {
			//其他情况说明不是utf-8
			return false;
		}
	}
	return true;
}

// 判断是否是gbk
bool IsGBK(unsigned char* data, int len) {
	int i = 0;
	while (i < len) {
		if (data[i] <= 0x7f) {
			//编码小于等于127,只有一个字节的编码，兼容ASCII
			i++;
			continue;
		}
		else {
			//大于127的使用双字节编码
			if (data[i] >= 0x81 &&
				data[i] <= 0xfe &&
				data[i + 1] >= 0x40 &&
				data[i + 1] <= 0xfe &&
				data[i + 1] != 0xf7) {
				i += 2;
				continue;
			}
			else {
				return false;
			}
		}
	}
	return true;
}