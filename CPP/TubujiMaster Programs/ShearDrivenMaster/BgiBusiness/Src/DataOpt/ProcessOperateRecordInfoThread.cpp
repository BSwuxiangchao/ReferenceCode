#include "ProcessOperateRecordInfoThread.h"
#include <QDateTime>
#include <QFile>
#include "BgiFileTools.h"
#include "CommonColors.h"

ProcessOperateRecordInfoThread::ProcessOperateRecordInfoThread()
	: m_bStop(true)
{
	// 文件夹路径
	m_strFilePath = "";
	// 文件存放记录文件的一级文件夹名称
	m_strOnePathName = "OperateLog";
	// 文件名称前缀
	m_strFilePrefixName = "Record";

}
ProcessOperateRecordInfoThread::~ProcessOperateRecordInfoThread()
{
	m_bStop = true;
}



// 启动线程
void ProcessOperateRecordInfoThread::StartThread()
{
	this->start();
}

// 停止线程
void ProcessOperateRecordInfoThread::StopThread()
{
	m_bStop = true;
	this->wait();
	this->quit();
}

void ProcessOperateRecordInfoThread::run()
{
	if (!m_bStop)
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
	OperateRecordInfo& recordInfo = OperateRecordInfo::GetInstance();
	QVector<RecordData> vec;
	bool bTrue = false;
	while (!m_bStop)
	{
		// 获取信息
		vec.clear();
		bTrue = OperateRecordInfo::GetInstance().GetRecordData(vec);
		if (bTrue)
		{
			QDateTime curTimeDate = QDateTime::currentDateTime();
			QString strDateTime = curTimeDate.toString("yyyy-MM-dd hh:mm:ss");
			QString strData = strDateTime + "->";
			QString strWrite = "";
			bool bWrite = true;
			for (int i = 0; i < vec.size(); ++i)
			{
				QString strSend = strData;
				strSend += vec.at(i).m_strModelName;
				strSend += ":";
				strSend += vec.at(i).m_strInfoType;
				strSend += ":";
				strSend += vec.at(i).m_strContent;
				//strSend += ";";
				// 发送数据到主线程
				if (vec.at(i).m_bShow)
				{
					emit SendRecordData(strSend, vec.at(i).m_color);
				}
				if (vec.at(i).m_bWrite == false)
					bWrite = false;
				// 记录下来以便存储到文件中
				strWrite.append(strSend);
				strWrite.append("\n");
			}
			// 写入到日志文件
			if(bWrite)
				WriteRecordData(strWrite);
		}
		QThread::msleep(1);
	}
}

// 写入记录文件
void ProcessOperateRecordInfoThread::WriteRecordData(const QString& strData)
{
	// 判断文件夹是否存在，不存在则创建之
	if (false == BgiFileTools::IsDirExist(m_strFilePath))
	{
		BgiFileTools::MakePath(m_strFilePath);
		if (false == BgiFileTools::IsDirExist(m_strFilePath))
		{
			// 发送数据到主线程
			emit SendRecordData(gc_strOperateLogMakeDirFail, gc_colorRed);
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
