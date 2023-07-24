#include "OpeStyleSheet.h"
#include "BgiFileTools.h"
#include <QFile>

OpeStyleSheet::OpeStyleSheet()
{
	// 默认路径
	// 创建路径
	QString path;
	BgiFileTools::GetDataDirPath(path);
	path.append("/Data/Input/StyleSheet");

	m_strFolderPath = path;
}
OpeStyleSheet::OpeStyleSheet(const QString& strFolderPath)
	: m_strFolderPath(strFolderPath)
{

}

OpeStyleSheet::~OpeStyleSheet()
{

}

// 通过文件名称读取控件样式表
bool OpeStyleSheet::ReadUICtrlsStyleSheetByFileName(QString& strStyleSheet, const QString fileName)
{
	QString strFilePath = "";
	strFilePath = QString("%1/%2.qss").arg(m_strFolderPath).arg(fileName);
	QFile file(strFilePath);
	if (false == file.open(QIODevice::ReadOnly))
	{
		return false;
	}
	strStyleSheet = file.readAll();

	file.close();
	return true;
}

// 读取QPushbutton样式
bool OpeStyleSheet::ReadQPushbuttonStyleSheet(QString& strStyleSheet, const unsigned short& iFontSize, const unsigned short& iType)
{
	QString strFilePath = "";
	if (iType == 0)
	{
		// 组合文件路径	
		strFilePath = QString("%1/QPushbutton-%2.qss").arg(m_strFolderPath).arg(iFontSize);
	}
	else if(iType == 1)
	{
		// 组合文件路径	
		strFilePath = QString("%1/QPushbuttonNoRadius-%2.qss").arg(m_strFolderPath).arg(iFontSize);
	}

	QFile file(strFilePath);
	if (false == file.open(QIODevice::ReadOnly))
	{
		return false;
	}
	strStyleSheet = file.readAll();
	
	file.close();
	return true;
}

// 读取QLineEdit样式
bool OpeStyleSheet::ReadQLineEditStyleSheet(QString& strStyleSheet, const unsigned short& iFontSize)
{
	// 组合文件路径
	QString strFilePath = QString("%1/QLineEdit-%2.qss").arg(m_strFolderPath).arg(iFontSize);

	QFile file(strFilePath);
	if (false == file.open(QFile::ReadOnly))
	{
		return false;
	}
	strStyleSheet = file.readAll();

	file.close();
	return true;
}
// 读取QGroupBox样式
bool OpeStyleSheet::ReadQGroupBoxStyleSheet(QString& strStyleSheet, const unsigned short& iFontSize)
{
	// 组合文件路径
	QString strFilePath = QString("%1/QGroupBox-%2.qss").arg(m_strFolderPath).arg(iFontSize);

	QFile file(strFilePath);
	if (false == file.open(QFile::ReadOnly))
	{
		return false;
	}
	strStyleSheet = file.readAll();

	file.close();
	return true;
}

// 设置样式表所在文件夹路径
void OpeStyleSheet::SetFolderPath(const QString& strFolderPath)
{
	m_strFolderPath = strFolderPath;
}


