/*
* @简    述：接收弹窗信息类；
* @详    细：接收弹窗信息类；
* @继    承：无
* @头 文 件：OperatePromptInfo.h
* @实现文件：OperatePromptInfo.cpp
* @上一版本：1.0
* @作    者：xiechengwang
* @创建时间：2022/8/10 11:22
* @当前版本：1.0
* @修改时间：无
*
*/

// 使用宏定义保护头文件不被重复包含
#pragma once
// 包含系统文件
#include <QString>
#include <QVector>
#include <QMutex>
#include <QColor>

// 包含项目文件
#include "BusinessExternDef.h"
#include "CommonColors.h"

// 包含局部头文件

// 放置前置引用

// 弹窗信息单元
struct BGIBUSINE_CLASS_EXPORT _tagPromptData
{
	// 标题
	QString m_strTitle;
	// 内容
	QString m_strContent;
	// 显示方式-模态/非模态
	bool m_bModel;
	// 设置按钮的显示,1-只显示确定按钮，2-只显示取消，3-显示确定和取消按钮，4-都不显示确定和取消，其他值默认只显示确认按钮
	int m_iBtnType;
	// 标题前景色
	QColor m_colForeground;
	// 标题背景色
	QColor m_colBackground;

	_tagPromptData();
	_tagPromptData& operator=(const _tagPromptData& obj);
	bool operator==(const _tagPromptData& obj) const;
};


class BGIBUSINE_CLASS_EXPORT OperatePromptInfo
{
public:
	// 这里放置类的生命期控制函数，例如构造，析构函数，拷贝构造函数

	static OperatePromptInfo& GetInstance()
	{
		static OperatePromptInfo instance;
		return instance;
	}

	// 这里放置类的运算操作符函数，=，==等

	// 这里放置类的操作

	// 添加数据-只能显示非模态弹窗
	static void AddPromptInfo(const QString& strTitle, const QString& strContent, const QColor& colForeground = Qt::blue, const QColor& colBackground = gc_colorLightGreen);
	// 添加数据-可选择模态或者非模态弹窗
	static void AddPromptInfo(const _tagPromptData& propmtData);
	// 获取数据
	static bool GetPromptData(QVector<_tagPromptData>& vecData);

	// 这里放置属性的存取

	// 这里放置类的状态查询

protected:

	// 互斥锁
	static QMutex m_mutex;
	// 信息列表
	static QVector<_tagPromptData> m_vecRecord;

private:
	OperatePromptInfo();
	~OperatePromptInfo();
	OperatePromptInfo(const OperatePromptInfo& obj) = delete;
	OperatePromptInfo& operator=(const OperatePromptInfo& obj) = delete;
};

