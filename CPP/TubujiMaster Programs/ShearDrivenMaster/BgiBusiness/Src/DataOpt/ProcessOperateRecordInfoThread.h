/*
* @简    述：处理接收操作记录信息线程类：
* @详    细：用于处理记录信息的显示和写入日志文件；
* @继    承：
* @头 文 件：
* @实现文件：
* @上一版本：1.0
* @作    者：xiechengwang
* @创建时间：2022/7/13 18:31
* @当前版本：1.0
* @修改时间：无
*
*/

// 使用宏定义保护头文件不被重复包含
#pragma once
// 包含系统文件
#include <QThread>

// 包含项目文件
#include "OperateRecordInfo.h"
#include "BusinessExternDef.h"

// 包含局部头文件

// 放置前置引用

class BGIBUSINE_CLASS_EXPORT ProcessOperateRecordInfoThread : public QThread
{
	Q_OBJECT
public:
	// 这里放置类的生命期控制函数，例如构造，析构函数，拷贝构造函数
	ProcessOperateRecordInfoThread();
	~ProcessOperateRecordInfoThread();

	// 这里放置类的运算操作符函数，=，==等

	// 这里放置类的操作

	// 启动线程
	void StartThread();

	// 停止线程
	void StopThread();

	// 写入记录文件
	void WriteRecordData(const QString& strData);

	// 这里放置属性的存取

	// 这里放置类的状态查询

signals:
	// 发送数据到主线程
	void SendRecordData(QString strContent, QColor col);

protected:
	virtual void run();

private:
	// 运行标志
	bool m_bStop;

	// 文件夹路径
	QString m_strFilePath;
	// 文件存放记录文件的一级文件夹名称
	QString m_strOnePathName;
	// 文件名称前缀
	QString m_strFilePrefixName;
};

