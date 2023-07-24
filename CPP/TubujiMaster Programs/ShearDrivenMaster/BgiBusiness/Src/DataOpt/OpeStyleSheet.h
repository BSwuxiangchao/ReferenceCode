/*
* @简    述：读取样式表内容
* @详    细：读取样式表内容
* @继    承：
* @头 文 件：
* @实现文件：
* @上一版本：1.1
* @作    者：wuxiangchao
* @创建时间：2023/4/17
* @当前版本：1.0
* @修改时间：无
*
*/

// 使用宏定义保护头文件不被重复包含
#pragma once
// 包含系统文件
#include <QString>

// 包含项目文件
#include "BusinessExternDef.h"

// 包含局部头文件

// 放置前置引用

class BGIBUSINE_CLASS_EXPORT OpeStyleSheet
{
public:
	// 这里放置类的生命期控制函数，例如构造，析构函数，拷贝构造函数
	OpeStyleSheet();
	OpeStyleSheet(const QString& strFolderPath);
	~OpeStyleSheet();
	// 这里放置类的运算操作符函数，=，==等

	// 这里放置类的操作

	// 通过文件名称读取控件样式表
	bool ReadUICtrlsStyleSheetByFileName(QString& strStyleSheet, const QString fileName);
	// 读取QPushbutton样式
	bool ReadQPushbuttonStyleSheet(QString& strStyleSheet, const unsigned short& iFontSize = 12,const unsigned short& iType=0);
	// 读取QLineEdit样式
	bool ReadQLineEditStyleSheet(QString& strStyleSheet, const unsigned short& iFontSize = 12);
	// 读取QGroupBox样式
	bool ReadQGroupBoxStyleSheet(QString& strStyleSheet, const unsigned short& iFontSize = 12);

	// 这里放置属性的存取
	// 设置样式表所在文件夹路径
	void SetFolderPath(const QString& strFolderPath);

	// 这里放置类的状态查询

protected:
	// 样式表所在文件夹路径
	QString m_strFolderPath;

private:
};

