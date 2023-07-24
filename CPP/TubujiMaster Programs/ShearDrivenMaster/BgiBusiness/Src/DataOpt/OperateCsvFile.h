/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 创建时间：2022/6/9 14:33
 * 功能描述：对.csv文件进行读写操作；
 ************************************* end-XCW *****************************************/
#pragma once
#include <QString>
#include <QVector>
#include "BusinessExternDef.h"


class BGIBUSINE_CLASS_EXPORT OperateCsvFile
{
public:
	OperateCsvFile();
	~OperateCsvFile();

/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 功能描述：读.csv文件内容
 * 时    间：2022/6/9 14:40
 * 入    参1：strPath：文件路径；
 * 出    参：lstHead:表头；
 * 出    参：vecStrList：表内容；
 * 返    回：true-成功，false-失败；
 ************************************* end-XCW *****************************************/
	static bool ReadFileContent(const QString& strPath, QStringList& lstHead, QVector<QStringList>& vecStrList);

/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 功能描述：写入文件;
 * 时    间：2022/6/9 14:40
 * 入    参1：strPath：文件路径；
 * 入    参2：vecStrList：写入表格内容，第一行是表头，其余为内容；
 * 出    参：无；
 * 返    回：true-成功，false-失败；
 ************************************* end-XCW *****************************************/
	static bool WriteFileContent(const QString& strPath, const QVector<QStringList>& vecStrList);

/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 功能描述：写入文件;
 * 时    间：2022/6/9 14:40
 * 入    参1：strPath：文件路径；
 * 入    参2：lstHead：写入表头；
 * 入    参3：vecStrList：写入表格内容；
 * 出    参：无；
 * 返    回：true-成功，false-失败；
 ************************************* end-XCW *****************************************/
	static bool WriteFileContent(const QString& strPath, const QStringList& lstHead, const QVector<QStringList>& vecStrList);

	// 追加文件内容
	static bool AppendFileContent(const QString& strPath, const QVector<QStringList>& vecStrList);

/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 功能描述： 获取文件后缀名;
 * 时    间：2022/6/9 14:40
 * 入    参1：strPath：文件路径；
 * 出    参：无；
 * 返    回：成功-文件后缀名，失败-空字符串；
 ************************************* end-XCW *****************************************/
	static QString GetFilePathSuffix(QString strPath);

	// 判断后缀是否为CSV文件
	static bool IsCsvFileSuffix(const QString& strSuffix);

protected:


private:
/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 功能描述：读.csv文件内容，第一行为表头，其余为内容；
 * 时    间：2022/6/9 14:40
 * 入    参1：strPath：文件路径；
 * 出    参：vecStrList：第一行为表头，其余为内容；
 * 返    回：true-成功，false-失败；
 ************************************* end-XCW *****************************************/
	static bool ReadFileContent(const QString& strPath, QVector<QStringList>& vecStrList);


};

