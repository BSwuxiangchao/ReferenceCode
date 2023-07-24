#include "OperatePromptInfo.h"


_tagPromptData::_tagPromptData()
{
	// 标题
	m_strTitle = "";
	// 内容
	m_strContent = "";
	// 显示方式-模态/非模态
	m_bModel = false;
	// 按钮类型
	m_iBtnType = 3;
	// 标题前景色
	m_colForeground = gc_colorBlue;
	// 标题背景色
	m_colBackground = gc_colorLightGreen;
}
_tagPromptData& _tagPromptData::operator=(const _tagPromptData& obj)
{
	if (this != &obj)
	{
		m_strTitle = obj.m_strTitle;
		m_strContent = obj.m_strContent;
		m_bModel = obj.m_bModel;
		m_iBtnType = obj.m_iBtnType;
		m_colForeground = obj.m_colForeground;
		m_colBackground = obj.m_colBackground;
	}
	return *this;
}
bool _tagPromptData::operator==(const _tagPromptData& obj) const
{
	if (m_strTitle == obj.m_strTitle && m_strContent == obj.m_strContent && m_bModel == obj.m_bModel &&	m_iBtnType == obj.m_iBtnType
		&&	m_colForeground == obj.m_colForeground && m_colBackground == obj.m_colBackground)
	{
		return true;
	}
	return false;
}


OperatePromptInfo::OperatePromptInfo()
{

}
OperatePromptInfo::~OperatePromptInfo()
{

}

// 互斥锁
QMutex OperatePromptInfo::m_mutex;
// 信息列表
QVector<_tagPromptData> OperatePromptInfo::m_vecRecord;

// 添加数据
void OperatePromptInfo::AddPromptInfo(const QString& strTitle, const QString& strContent, const QColor& colForeground, const QColor& colBackground)
{
	QMutexLocker locker(&m_mutex);
	_tagPromptData data;
	data.m_strTitle = strTitle;
	data.m_strContent = strContent;
	data.m_bModel = false;
	data.m_iBtnType = 1;
	data.m_colForeground = colForeground;
	data.m_colBackground = colBackground;

	m_vecRecord.push_back(data);
}

// 添加数据-可选择模态或者非模态弹窗
void OperatePromptInfo::AddPromptInfo(const _tagPromptData& propmtData)
{
	QMutexLocker locker(&m_mutex);
	m_vecRecord.push_back(propmtData);
}
// 获取数据
bool OperatePromptInfo::GetPromptData(QVector<_tagPromptData>& vecData)
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