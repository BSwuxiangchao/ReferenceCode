/*
* @Abrief 文件目录操作
* @note 封装Qt接口，提供目录、文件操作合集
* @version 1.0
* @author mosongjin 2021-07-21
*/

#ifndef _BGI_FILE_TOOLS_H
#define _BGI_FILE_TOOLS_H

#include <QVector>
#include "ExternDef.h"

class QString;
class BGIBASE_CLASS_EXPORT BgiFileTools
{
public:
	// 复制文件、目录
	static bool CopyFile(const QString& strFileFrom, QString& strFileTo, bool bOverride = true);
	static bool CopyDirectory(const QString& strDirFrom, const QString& strDirTo, bool bOverride = true);

	// 删除文件、目录
	static bool DeleteFile(const QString& strFile);
	static bool DeleteDirectory(const QString& strDir);

	// 获取文件、目录大小
	static quint64 GetFileSize(const QString& strFile);
	static quint64 GetDirectorySize(const QString& strDir);
	// 获取目录信息
	static bool GetDirFiles(const QString& strPath, QVector<QString>& files);
	// 获取文件后缀名
	static bool GetFileSuffix(const QString& strFileFull, QString& strFileSuffix);

	// 文件判断
	static bool IsFileExist(const QString& strFile);

	// 目录判断
	static bool IsDirectory(const QString& strPath);
	static bool IsDirExist(const QString& strPath);

	// 拼接路径信息
	static QString GetFullPath(const QString& strBasePath, const QString& strSubPath);

	// 创建目录
	static bool MakePath(const QString& strPath);

	//获取指定目录下所有后缀为Suffix的文件
	static void GetFilesWithCertainSuffix(const QString& dir, const QString& Suffix, QStringList& list);

	//查询指定目录下包括子目录下是否存在目标文件,若包含则获取目标文件fullpath
	static bool GetFileByFileName(const QString& filename, const QString& dir, QString& filefullpath);

	//查询Data资源所在的目录
	static void GetDataDirPath(QString& path);
};


#endif