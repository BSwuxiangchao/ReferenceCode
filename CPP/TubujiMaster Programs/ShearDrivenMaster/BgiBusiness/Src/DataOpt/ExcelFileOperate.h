/*
* @简    述：提供wps库读写Excel文件，offie库读写Excel文件；
* @详    细：提供wps库读写Excel文件，offie库读写Excel文件；
* @继    承：无
* @头 文 件：ExcelFileOperate.h
* @实现文件：ExcelFileOperate.cpp
* @上一版本：1.0
* @作    者：xiechengwang
* @创建时间：2022/2/23 18:23
* @当前版本：1.0
* @修改时间：无
*
*/

// 使用宏定义保护头文件不被重复包含
#pragma once
// 包含系统文件
#include <QVector>
#include "BusinessExternDef.h"

// 包含项目文件

// 包含局部头文件

// 放置前置引用

class BGIBUSINE_CLASS_EXPORT ExcelFileOperate
{
public:
	// 这里放置类的生命期控制函数，例如构造，析构函数，拷贝构造函数

	// 这里放置类的运算操作符函数，=，==等

	// 这里放置类的操作
	
	
/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 功能描述：读取Xecel文件
 * 时    间：2022/2/23 19:29
 * 入    参：pathName：路径
 * 入    参：Header：表头
 * 入    参：iType：读取库类型，1-office，2-wps，默认是office
 * 入    参：strReadInfo：读取状态
 * 出    参：vecContent：读取后的数据
 * 返    回：false-失败，true-成功
 ************************************* end-XCW *****************************************/
	bool ReadExcelFile(const QString& pathName, const QStringList& Header, QVector<QStringList>& vecContent, const int& iType, QString& strReadInfo);
	// 写文件
	bool WriteExcelFile(const QString& pathName, const QStringList& Header, const QVector<QStringList>& vecContent, const int& iType);

	// 这里放置属性的存取

	// 这里放置类的状态查询

protected:

private:
};

