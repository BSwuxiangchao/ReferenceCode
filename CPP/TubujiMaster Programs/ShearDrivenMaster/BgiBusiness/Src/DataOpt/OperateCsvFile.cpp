#include "OperateCsvFile.h"
#include <QFile>
#include <QTextStream>
#include "TransformCoding.h"

OperateCsvFile::OperateCsvFile()
{

}
OperateCsvFile::~OperateCsvFile()
{

}

/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 功能描述：读文件;
 * 时    间：2022/6/9 14:40
 * 入    参1：strPath：文件路径；
 * 入    参2：vecStrList：读取出来的内容；
 * 出    参：无；
 * 返    回：true-成功，false-失败；
 ************************************* end-XCW *****************************************/
bool OperateCsvFile::ReadFileContent(const QString& strPath, QVector<QStringList>& vecStrList)
{
	// 路径判断是否为.csv文件
	// 只读方式打开文件
	// 逐行读取内容
	// 关闭文件
	if (false == IsCsvFileSuffix(GetFilePathSuffix(strPath)))
	{
		// 不是csv文件
		return false;
	}
	// 清空
	vecStrList.clear();

	QFile readFile(strPath);
	if (!readFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		// 打开失败
		return false;
	}

	// 先读取一行，判断其编码
	QByteArray byteArray = readFile.readLine();
	// 设置编码再读取数据
	QStringConverter::Encoding curEncoding = QStringConverter::System;
	if (IsUTF8(byteArray, byteArray.size()))
	{
		curEncoding = QStringConverter::Utf8;
	}
	// 关闭文件
	readFile.close();

	// 再次打开文件
	readFile.setFileName(strPath);
	if (!readFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		// 打开失败
		return false;
	}

	// 读取
	QStringList strList;
	QTextStream in(&readFile);
	in.setEncoding(curEncoding);  // 设置编码

	while (!in.atEnd())
	{
	    QString strLine = in.readLine();
		// 移除换行符号
//		strLine.chop(1); 
		strList = strLine.split(",");  // 根据","分开每行的列
		vecStrList.push_back(strList);
	};

	// 关闭
	readFile.close();

	return true;
}

bool OperateCsvFile::ReadFileContent(const QString& strPath, QStringList& lstHead, QVector<QStringList>& vecStrList)
{
	// 路径判断是否为.csv文件
	// 只读方式打开文件
	// 逐行读取内容
	// 关闭文件
	if (false == IsCsvFileSuffix(GetFilePathSuffix(strPath)))
	{
		// 不是csv文件
		return false;
	}
	// 清空
	vecStrList.clear();

	QFile readFile(strPath);
	if (!readFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		// 打开失败
		return false;
	}

	// 先读取一行，判断其编码
	QByteArray byteArray = readFile.readLine();
	// 设置编码再读取数据
	QStringConverter::Encoding curEncoding = QStringConverter::System;
	if (IsUTF8(byteArray, byteArray.size()))
	{
		curEncoding = QStringConverter::Utf8;
	}
	// 关闭文件
	readFile.close();

	// 再次打开文件
	readFile.setFileName(strPath);
	if (!readFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		// 打开失败
		return false;
	}

	// 读取
	QStringList strList;
	QString strLine;
	QTextStream in(&readFile);
	in.setEncoding(curEncoding);  // 设置编码

	// 读取表头
	strLine = in.readLine(); // QTextStream读取出来的自动去掉'\n'; readFile.readLine();带换行符'\n'；
	// 移除换行符号
//	strLine.chop(1);  
	strList = strLine.split(",");  // 根据","分开每行的列
	lstHead = strList;

	while (!in.atEnd())
	{
		strLine = in.readLine();
		// 移除换行符号
//		strLine.chop(1);
		strList = strLine.split(",");  // 根据","分开每行的列
		vecStrList.push_back(strList);
	};

	// 关闭
	readFile.close();

	return true;
}

/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 功能描述：写入文件;
 * 时    间：2022/6/9 14:40
 * 入    参1：strPath：文件路径；
 * 入    参2：vecStrList：写入表格内容，第1行为表头，其余为表格内容；
 * 出    参：无；
 * 返    回：true-成功，false-失败；
 ************************************* end-XCW *****************************************/
bool OperateCsvFile::WriteFileContent(const QString& strPath, const QVector<QStringList>& vecStrList)
{
	if (false == IsCsvFileSuffix(GetFilePathSuffix(strPath)))
	{
		// 不是csv文件
		return false;
	}

	QFile wFile(strPath);

	if (!wFile.open(QIODevice::WriteOnly))
	{
		return false;
	}

	// 设置写入编码
	QTextStream out(&wFile);
	out.setEncoding(QStringConverter::System);  // 设置本地编码写入文件

	for (int i = 0; i < vecStrList.size(); ++i)
	{
		QString strLine = vecStrList.at(i).join(",");
		strLine += "\n";
		out << strLine;
	}

	wFile.close();

	return true;
}

bool OperateCsvFile::WriteFileContent(const QString& strPath, const QStringList& lstHead, const QVector<QStringList>& vecStrList)
{
	if (false == IsCsvFileSuffix(GetFilePathSuffix(strPath)))
	{
		// 不是csv文件
		return false;
	}

	QFile wFile(strPath);

	if (!wFile.open(QIODevice::WriteOnly))
	{
		return false;
	}

	QTextStream out(&wFile);
	out.setEncoding(QStringConverter::System);  // 使用本地编码写入文件

	QString strLine;
	// 写入表头
	strLine = lstHead.join(",");
	strLine += "\n";
	out << strLine;

	// 写入内容
	for (int i = 0; i < vecStrList.size(); ++i)
	{
		strLine = vecStrList.at(i).join(",");
		strLine += "\n";
		out << strLine;
	}

	wFile.close();

	return true;
}

// 追加文件内容
bool OperateCsvFile::AppendFileContent(const QString& strPath, const QVector<QStringList>& vecStrList)
{
	if (false == IsCsvFileSuffix(GetFilePathSuffix(strPath)))
	{
		// 不是csv文件
		return false;
	}

	QFile wFile(strPath);

	if (!wFile.open(QIODevice::Append))
	{
		return false;
	}

	// 设置写入编码
	QTextStream out(&wFile);
	out.setEncoding(QStringConverter::System);  // 设置本地编码写入文件

	for (int i = 0; i < vecStrList.size(); ++i)
	{
		QString strLine = vecStrList.at(i).join(",");
		strLine += "\n";
		out << strLine;
	}

	wFile.close();

	return true;
}

/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 功能描述： 获取文件后缀名;
 * 时    间：2022/6/9 14:40
 * 入    参1：strPath：文件路径；
 * 出    参：无；
 * 返    回：成功-文件后缀名，失败-空字符串；
 ************************************* end-XCW *****************************************/
QString OperateCsvFile::GetFilePathSuffix(QString strPath)
{
	// 查找最后一个"."
	// 截取最后一个"."到结束的所有字符串
	int iLastIndex = strPath.lastIndexOf(".");
	if (0 > iLastIndex)
	{
		// 没找到
		return "";
	}
	
	return strPath.right(strPath.size() - iLastIndex - 1);
}

// 判断后缀是否为CSV文件
bool OperateCsvFile::IsCsvFileSuffix(const QString& strSuffix)
{
	int iLength = strSuffix.size();
	if (3 != iLength)
	{
		return false;
	}

	char ch = strSuffix.at(0).toLatin1();
	if ('c' == ch || 'C' == ch)
	{
	}
	else
	{
		return false;
	}

	ch = strSuffix.at(1).toLatin1();
	if ('s' == ch || 'S' == ch)
	{
	}
	else
	{
		return false;
	}

	ch = strSuffix.at(2).toLatin1();
	if ('v' == ch || 'V' == ch)
	{
	}
	else
	{
		return false;
	}

	return true;
}