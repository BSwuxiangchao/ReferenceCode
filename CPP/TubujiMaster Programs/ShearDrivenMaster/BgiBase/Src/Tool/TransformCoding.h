/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 创建时间：2022/1/13 9:27
 * 功能描述：提供字符串的编码转换函数；
 ************************************* end-XCW *****************************************/
#pragma once
#include <QString>
#include "ExternDef.h"

typedef enum _tagFileCodeing 
{
	FILE_CODEING_GB2312 = 1,
	FILE_CODEING_UTF_8 = 2,
}FileCodeing;


/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 功能描述：将源文件输入的中文字符（乱码）转换为QString类型的正常中文；
 * 时    间：2022/1/13 9:30
 * 入    参：data：需要转换的字符串；
 * 入    参：type：字符串所在源文件的编码类型（1-（系统默认）GB2312代码页936, 2-Unicode带标签/无标签代码页65001，其他值则返回空）；
 * 出    参：无
 * 返    回：转换后的QString字符串
 ************************************* end-XCW *****************************************/
QString BGIBASE_CAPI_EXPORT StringConversionEncoding(const char* data, FileCodeing type);
QString BGIBASE_CAPI_EXPORT StringConversionEncoding(const char* data);

/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 功能描述：判断字符串是否为UTF-8
 * 时    间：2022/1/17 20:57
 * 入    参：str：判断的字符串；
 * 入    参：len：字符串长度
 * 出    参：无
 * 返    回：是-true，否-false
 ************************************* end-XCW *****************************************/
bool BGIBASE_CAPI_EXPORT IsUTF8(const char* str, int len);


