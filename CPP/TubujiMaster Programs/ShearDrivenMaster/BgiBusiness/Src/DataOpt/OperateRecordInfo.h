/*
* @简    述：接收记录数据类
* @详    细：提供一个函数接收各个模块需要显示的数据
* @继    承：无
* @头 文 件：OperateRecordInfo.h
* @实现文件：OperateRecordInfo.cpp
* @上一版本：1.0
* @作    者：xiechengwang
* @创建时间：2022/7/13 16:48
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
#include "SortingParaCommonName.h"

// 包含项目文件
#include "BusinessExternDef.h"

// 包含局部头文件

// 放置前置引用
// 
// 记录数据结构体
struct BGIBUSINE_CLASS_EXPORT RecordData
{
	// 模块名称
	QString m_strModelName;
	// 信息类型名称
	QString m_strInfoType;
	// 信息内容
	QString m_strContent;
	// 显示的颜色，默认黑色
	QColor m_color;
	// 显示到界面-默认显示
	bool m_bShow;
	// 写到日志文件
	bool m_bWrite;

	RecordData();

	RecordData& operator=(const RecordData& obj);

	bool operator==(const RecordData& obj) const;
};

class BGIBUSINE_CLASS_EXPORT OperateRecordInfo
{
public:

	// 获取对象实例
	static OperateRecordInfo& GetInstance()
	{
		static OperateRecordInfo instance;
		return instance;
	}

	// 这里放置类的操作

	// 添加数据
	static void AddRecordData(const RecordData& obj);

	// 获取数据
	static bool GetRecordData(QVector<RecordData>& vecData);

protected:
	// 互斥锁
	static QMutex m_mutex;
	// 信息列表
	static QVector<RecordData> m_vecRecord;

private:
	// 这里放置类的生命期控制函数，例如构造，析构函数，拷贝构造函数
	OperateRecordInfo();
	~OperateRecordInfo();
	OperateRecordInfo(const OperateRecordInfo&) = delete;
	OperateRecordInfo& operator=(const OperateRecordInfo&) = delete;
};

