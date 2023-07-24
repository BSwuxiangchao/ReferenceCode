#include "BgiFileTools.h"
#include <QString>
#include <QDir>
#include <QDirIterator>
#include "BgiLogger.h"
#include <qcoreapplication.h>

/**
 * @brief 复制文件至指定路径
 * @param strFileFrom			复制源文件
 * @param strFileTo				复制目标文件
 * @param bOverride				复制目标文件是否已存在；若true，则覆盖
 * @return 函数执行结果，成功返回true
 */
bool BgiFileTools::CopyFile(const QString& strFileFrom, QString& strFileTo, bool bOverride)
{
	// 目录分隔符转换（兼容Linux操作系统）
	QString str_file_from_tmp = QDir::fromNativeSeparators(strFileFrom);
	QString str_file_to_tmp = QDir::fromNativeSeparators(strFileTo);

	// 源文件和目标文件是否为同一个文件
	if (str_file_from_tmp == str_file_to_tmp)
	{
		BGI_MESSAGE("[%d][%s][Source and destination files are identical:%s]", __LINE__, __FUNCTION__, str_file_from_tmp.toStdString().c_str());
		return true;
	}

	// 源文件是否存在
	if (!QFile::exists(str_file_from_tmp))
	{
		BGI_ERROR("[%d][%s][Source file is not exist:%s]", __LINE__, __FUNCTION__, str_file_from_tmp.toStdString().c_str());
		return false;
	}

	QDir dir;
	bool b_exist = dir.exists(str_file_to_tmp);
	// 目标文件存在
	if (b_exist)
	{
		// 目标文件已存在，是否删除
		if (bOverride)
		{
			dir.remove(str_file_to_tmp);
		}
	}

	// 拷贝文件
	if (!QFile::copy(str_file_from_tmp, str_file_to_tmp))
	{
		BGI_ERROR("[%d][%s][Copy file failed:%s]", __LINE__, __FUNCTION__, str_file_from_tmp.toStdString().c_str());
		return false;
	}

	return true;
}

/**
 * @brief 复制目录至指定路径
 * @param strDirFrom			复制源目录路径
 * @param strDirTo				复制目标路径
 * @param bOverride				复制目标文件是否已存在；若true，则删除后复制
 * @return 函数执行结果，成功返回true
 */
bool BgiFileTools::CopyDirectory(const QString& strDirFrom, const QString& strDirTo, bool bOverride)
{
	// 目录分隔符转换（兼容Linux操作系统）
	QDir dir_tmp_from = QDir::fromNativeSeparators(strDirFrom);
	QDir dir_tmp_to = QDir::fromNativeSeparators(strDirTo);

	// 目录不存在，创建目录
	if (!dir_tmp_to.exists())
	{
		if (!dir_tmp_to.mkpath(dir_tmp_to.absolutePath()))
		{
			BGI_ERROR("[%d][%s][Make path failed:%s]", __LINE__, __FUNCTION__, dir_tmp_to.dirName().toStdString().c_str());
			return false;
		}
	}

	// 递归复制目录文件
	QFileInfoList file_info_list = dir_tmp_from.entryInfoList();
	foreach(QFileInfo file_info, file_info_list)
	{
		if (file_info.fileName() == "." || file_info.fileName() == "..")
		{
			continue;
		}

		if (file_info.isDir())
		{ // 拷贝子目录
			if (!CopyDirectory(file_info.filePath(), dir_tmp_to.filePath(file_info.fileName()), bOverride))
			{
				BGI_ERROR("[%d][%s][Copy dir failed:%s]", __LINE__, __FUNCTION__, file_info.fileName().toStdString().c_str());
				return false;
			}
		}
		else
		{ // 拷贝文件
			if (bOverride && dir_tmp_to.exists(file_info.fileName()))
			{
				dir_tmp_to.remove(file_info.fileName());
			}
			if (!QFile::copy(file_info.filePath(), dir_tmp_to.filePath(file_info.fileName())))
			{
				BGI_ERROR("[%d][%s][Copy file failed:%s]", __LINE__, __FUNCTION__, file_info.fileName().toStdString().c_str());
				return false;
			}
		}
	}

	return true;
}

/**
 * @brief 删除文件
 * @param strFile 删除文件全路径名称
 * @return 函数执行结果
 */
bool BgiFileTools::DeleteFile(const QString& strFile)
{
	// 源文件是否存在
	if (!QFile::exists(QDir::fromNativeSeparators(strFile)))
	{
		BGI_ERROR("[%d][%s][Source file is not exist:%s]", __LINE__, __FUNCTION__, strFile.toStdString().c_str());
		return false;
	}

	return QFile::remove(QDir::fromNativeSeparators(strFile));
}

/**
 * @brief 删除目录
 * @param strDir 删除目录路径
 * @return 函数执行结果，成功返回true
 */
bool BgiFileTools::DeleteDirectory(const QString& strDir)
{
	// 目录分隔符转换（兼容Linux操作系统）
	QString str_dir_tmp = QDir::fromNativeSeparators(strDir);

	QDir dir(str_dir_tmp);
	// 目录不存在，删除成功
	if (!dir.exists())
	{
		BGI_MESSAGE("[%d][%s][Delete dir is not exist:%s]", __LINE__, __FUNCTION__, dir.dirName().toStdString().c_str());
		return true;
	}

	// 递归删除目录
	QString str_tmp_dir;
	QFileInfoList file_list = dir.entryInfoList();
	foreach(QFileInfo file_info, file_list) 
	{
		if (file_info.fileName() == "." || file_info.fileName() == "..")
		{
			continue;
		}

		if (file_info.isDir())
		{ // 删除子目录
			str_tmp_dir = str_dir_tmp + ("/") + file_info.fileName();
			DeleteDirectory(str_tmp_dir);
			dir.rmdir(file_info.fileName());
		}
		else if (file_info.isFile())
		{ // 删除文件
			QFile tmpFile(file_info.fileName());
			dir.remove(tmpFile.fileName());
		}
		else
		{
			// nothing to do.
		}
	}

	if (QDir().exists(str_dir_tmp))
	{
		// 返回上级目录，因为只有返回上级目录，才可以删除这个目录
		QString str_dir_name = dir.dirName();
		dir.cdUp();
		if (!dir.rmdir(str_dir_name))
		{
			BGI_ERROR("[%d][%s][Delete dir failed:%s]", __LINE__, __FUNCTION__, str_dir_name.toStdString().c_str());
			return false;
		}
	}

	return true;
}

/**
 * @brief 获取文件大小（单位字节）
 * @param strFile 文件全路径名
 * @return 函数执行结果
 */
quint64 BgiFileTools::GetFileSize(const QString& strFile)
{
	// 目录分隔符转换（兼容Linux操作系统）
	QFile file(QDir::fromNativeSeparators(strFile));

	quint64 n_size = 0;
	if (file.open(QIODevice::ReadOnly))
	{
		n_size = file.size();
	}
	file.close();
	return n_size;
}

/**
 * @brief 获取目录大小
 * @param strDir 目录全路径
 * @return 函数执行结果
 */
quint64 BgiFileTools::GetDirectorySize(const QString& strDir)
{
	// 目录分隔符转换（兼容Linux操作系统）
	QString str_dir_tmp = QDir::fromNativeSeparators(strDir);

	QDir dir(str_dir_tmp);
	quint64 n_size = 0;

	// 遍历子目录名称
	foreach(QFileInfo file_info, dir.entryInfoList(QDir::Files))
	{
		n_size += file_info.size();
	}

	// 递归获取子目录大小
	foreach(QString sub_dir, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
	{
		QString str_sub_path = GetFullPath(str_dir_tmp, sub_dir);
		n_size += GetDirectorySize(str_sub_path);
	}

	return n_size;
}

/**
 * @brief 获取路径文件和目录信息（仅限于此路径，不包含子目录）
 * @param strPath			需要获取信息路径
 * @param files				获取信息
 * @return 函数执行结果，成功返回true
 */
bool BgiFileTools::GetDirFiles(const QString& strPath, QVector<QString>& files)
{
	// 目录分隔符转换（兼容Linux操作系统）
	QDir dir(QDir::fromNativeSeparators(strPath));

	// 路径不存在，返回false
	if (!dir.exists())
	{
		BGI_ERROR("[%d][%s][Dir is not exist:%s]", __LINE__, __FUNCTION__, dir.dirName().toStdString().c_str());
		return false;
	}

	// 遍历当前路径文件名称
	QFileInfoList file_list = dir.entryInfoList();
	foreach(QFileInfo fileInfo, file_list)
	{
		// 去除"."和".."目录
		if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
		{
			continue;
		}

		files.push_back(fileInfo.fileName());
	}

	return true;
}

/**
 * @brief 获取文件后缀名
 * @param strFileFull			文件全路径名
 * @param strFileSuffix			获取的后缀名
 * @return 函数执行结果，成功返回true
 */
bool BgiFileTools::GetFileSuffix(const QString& strFileFull, QString& strFileSuffix)
{
	// 目录分隔符转换（兼容Linux操作系统）
	QString str_file_full_tmp = QDir::fromNativeSeparators(strFileFull);

	QFileInfo file_info = QFileInfo(str_file_full_tmp);
	// 文件不存在，返回false
	if (!file_info.exists())
	{
		BGI_ERROR("[%d][%s][Source file is not exist:%s]", __LINE__, __FUNCTION__, str_file_full_tmp.toStdString().c_str());
		return false;
	}

	// 赋值后缀名
	strFileSuffix = file_info.suffix();
	return true;
}

bool BgiFileTools::IsFileExist(const QString& strFile)
{
	// 源文件是否存在
	return QFile::exists(strFile);
}

/**
 * @brief 是否为目录
 * @param strPath 判断路径
 * @return 函数执行结果，成功返回true
 */
bool BgiFileTools::IsDirectory(const QString& strPath)
{
	QDir dir(QDir::fromNativeSeparators(strPath));
	if (!dir.exists())
	{
		return false;
	}

	return true;
}

/**
 * @brief 目录是否存在
 * @param strPath 判断路径
 * @return 函数执行结果，成功返回true
 */
bool BgiFileTools::IsDirExist(const QString& strPath)
{
	bool bRet = true;

	// 目录分隔符转换（兼容Linux操作系统）
	QDir dir(QDir::fromNativeSeparators(strPath));
	// 路径不存在，返回false
	if (!dir.exists())
	{
		bRet = false;
	}

	return bRet;
}

/**
 * @brief 组合路径
 * @param strBasePath			父目录
 * @parame strSubPath			子名称
 * @return 函数执行结果
 */
QString BgiFileTools::GetFullPath(const QString& strBasePath, const QString& strSubPath)
{
	// 目录分隔符转换（兼容Linux操作系统）
	QString str_path = QDir::fromNativeSeparators(strBasePath);

	if (!str_path.isEmpty())
	{
		if (!str_path.endsWith("/"))
		{
			str_path.append("/");
		}
		str_path.append(strSubPath);
	}

	return QDir::fromNativeSeparators(str_path);
}

/**
 * @brief 组合路径
 * @param strPath				目标目录
 * @return 函数执行结果
 */
bool BgiFileTools::MakePath(const QString& strPath)
{
	// 目录分隔符转换（兼容Linux操作系统）
	QDir dir = QDir::fromNativeSeparators(strPath);

	// 目录不存在，创建目录
	if (!dir.exists())
	{
		if (!dir.mkpath(dir.absolutePath()))
		{
			BGI_ERROR("[%d][%s][Make path failed:%s]", __LINE__, __FUNCTION__, dir.dirName().toStdString().c_str());
			return false;
		}
	}

	return true;
}

void BgiFileTools::GetFilesWithCertainSuffix(const QString& dir, const QString& Suffix, QStringList& list)
{
	QDir Dir(dir);
	if (!Dir.exists())
	{
		return;
	}

	//获取所选文件类型过滤器 
	QStringList filters;
	filters << QString("*." + Suffix);

	//定义迭代器并设置过滤器 
	QDirIterator dir_iterator(dir, filters, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
	while (dir_iterator.hasNext())
	{
		dir_iterator.next();
		QFileInfo file_info = dir_iterator.fileInfo();
		QString absolute_file_path = file_info.absoluteFilePath();
		list.append(absolute_file_path);
	}
}

bool BgiFileTools::GetFileByFileName(const QString& filename, const QString& dir, QString& filefullpath)
{
	bool res = false;
	QDir Dir(dir);
	if (!Dir.exists())
	{
		return false;
	}

	Dir.setFilter(QDir::Dirs | QDir::Files | QDir::Filter::NoDotAndDotDot);//除了目录或文件，其他的过滤掉
	Dir.setSorting(QDir::DirsLast);//优先显示目录
	QFileInfoList list = Dir.entryInfoList();//获取文件信息列表
	int i = 0;
	while (i < list.size())
	{
		QFileInfo info = list.at(i);
		if (info.isDir())
		{
			res = GetFileByFileName(filename, info.filePath(), filefullpath);
		}
		if (res)
		{
			break;
		}
		if (info.fileName() == filename)
		{
			//filefullpath = info.absoluteFilePath();
			filefullpath = info.absolutePath();
			return true;
		}
		++i;
	}
	return res;
}

void BgiFileTools::GetDataDirPath(QString& path)
{
	QString fileName = QCoreApplication::applicationDirPath();
	QDir cur_path(fileName);
	cur_path.cdUp();
	cur_path.cdUp();
	cur_path.cdUp();
	cur_path.cdUp();
	path = cur_path.absolutePath();
}