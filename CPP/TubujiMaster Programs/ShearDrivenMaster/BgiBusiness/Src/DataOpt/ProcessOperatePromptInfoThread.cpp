#include "ProcessOperatePromptInfoThread.h"
#include <QDateTime>
#include <QFile>
#include "BgiFileTools.h"
#include "SortingParaCommonName.h"

ProcessOperatePromptInfoThread::ProcessOperatePromptInfoThread()
	: m_bStop(true)
{
	// 文件夹路径
	m_strFilePath = "";
	// 文件存放记录文件的一级文件夹名称
	m_strOnePathName = "OperateLog";
	// 文件名称前缀
	m_strFilePrefixName = "Record";
}

ProcessOperatePromptInfoThread::~ProcessOperatePromptInfoThread()
{
	m_bStop = true;
}

// 启动线程
void ProcessOperatePromptInfoThread::StartThread()
{
	this->start();
}


// 停止线程
void ProcessOperatePromptInfoThread::StopThread()
{
	m_bStop = true;
	this->wait();
	this->quit();
}

void ProcessOperatePromptInfoThread::run()
{
	if (false == m_bStop)
	{
		// 已经启动则直接返回
		return;
	}
	m_bStop = false;

	// 创建路径
	QString path;
	BgiFileTools::GetDataDirPath(path);
	path.append("/Data/Export/");
	path.append(m_strOnePathName);
	path.append("/");

	m_strFilePath = path;
	//创建目录
	BgiFileTools::MakePath(m_strFilePath);

	// 获取记录信息对象
	OperatePromptInfo& recordInfo = OperatePromptInfo::GetInstance();
	QVector<_tagPromptData> vec;
	bool bTrue = false;
	while (!m_bStop)
	{
		// 获取信息
		vec.clear();
		bTrue = recordInfo.GetPromptData(vec);
		if (bTrue)
		{
			QDateTime curTimeDate = QDateTime::currentDateTime();
			QString strDateTime = curTimeDate.toString("hh:mm:ss");
			QString strData = strDateTime + "->";
			QString strWrite = "";
			for (int i = 0; i < vec.size(); ++i)
			{
				QString strSend = strData;
				strSend += "弹窗消息:";
				strSend += vec.at(i).m_strTitle;
				strSend += ":";
				strSend += vec.at(i).m_strContent;
				strSend += ";";
				// 发送数据到主线程
				emit SendRecordData(vec.at(i));

				// 记录下来以便存储到文件中
				strWrite.append(strSend);
				strWrite.append("\n");
			}
			// 写入到日志文件
			WriteRecordData(strWrite);

		}
		QThread::msleep(10);
	}
}

// 写入记录文件
void ProcessOperatePromptInfoThread::WriteRecordData(const QString& strData)
{
	// 判断文件夹是否存在，不存在则创建之
	if (false == BgiFileTools::IsDirExist(m_strFilePath))
	{
		BgiFileTools::MakePath(m_strFilePath);
		if (false == BgiFileTools::IsDirExist(m_strFilePath))
		{
			// 发送数据到主线程
//			emit SendRecordData(gc_strOperateLogMakeDirFail, gc_colorRed);
			return;
		}
	}

	// 以时间创建文件名
	QDateTime curTimeDate = QDateTime::currentDateTime();
	QString strDateTime = curTimeDate.toString("yyyy-MM-dd");
	QString strFileName = m_strFilePrefixName + "_" + strDateTime + ".txt";
	QString strFilePath = m_strFilePath + strFileName;

	// 已追加方式写入文件
	QFile fileWrite(strFilePath);
	if (!fileWrite.open(QIODeviceBase::Append))
	{
		return;
	}
	// 写入
	fileWrite.write(strData.toUtf8());

	// 关闭
	fileWrite.close();
}

