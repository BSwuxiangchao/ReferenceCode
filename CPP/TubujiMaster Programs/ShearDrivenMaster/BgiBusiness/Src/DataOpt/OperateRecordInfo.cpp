#include "OperateRecordInfo.h"
#include "CommonColors.h"

// 记录数据结构体
RecordData::RecordData()
{
	m_strModelName = "";
	m_strInfoType = "";
	m_strContent = "";
	m_color = gc_colorBlack;
	m_bShow = true;
	m_bWrite = false;
}

RecordData& RecordData::operator=(const RecordData& obj)
{
	if (this != &obj)
	{
		m_strModelName = obj.m_strModelName;
		m_strInfoType = obj.m_strInfoType;
		m_strContent = obj.m_strContent;
		m_color = obj.m_color;
	}
	return *this;
}

bool RecordData::operator==(const RecordData& obj) const
{
	if (m_strModelName == obj.m_strModelName && m_strInfoType == obj.m_strInfoType && m_strContent == obj.m_strContent && m_color == obj.m_color)
	{
		return true;
	}
	return false;
}


OperateRecordInfo::OperateRecordInfo()
{

}

OperateRecordInfo::~OperateRecordInfo()
{

}

// 互斥锁
QMutex OperateRecordInfo::m_mutex;
// 信息列表
QVector<RecordData> OperateRecordInfo::m_vecRecord;

// 添加数据
void OperateRecordInfo::AddRecordData(const RecordData& obj)
{
	QMutexLocker locker(&m_mutex);
	m_vecRecord.push_back(obj);
}

// 获取数据
bool OperateRecordInfo::GetRecordData(QVector<RecordData>& vecData)
{
	if (m_vecRecord.isEmpty())
	{
		return false;
	}

	QMutexLocker locker(&m_mutex);

	vecData = m_vecRecord;

	// 清空
	m_vecRecord.clear();

	return true;
}