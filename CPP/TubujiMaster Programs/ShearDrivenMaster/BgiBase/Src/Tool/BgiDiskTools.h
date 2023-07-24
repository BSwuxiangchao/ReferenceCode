/*
* @Abrief 硬盘信息
* @note 获得硬盘的基础信息,Win32实现
* @version 1.0
* @author mosongjin 2021-07-21
*/

#ifndef _BGI_DISK_TOOLS_H
#define _BGI_DISK_TOOLS_H

#include "ExternDef.h"
#include <QString>
#include <QList>
#include <QMap>
#include <QObject>
#include "DiskDef.h"

#ifdef WIN32
#include <TCHAR.H>
#include <windows.h>
#endif // WIN32

class BGIBASE_CLASS_EXPORT DiskData
{
public:
	DiskData() : m_LocalFix("Local"),
		m_UsbFix("USB"),
		m_CdFix("CD")
	{};
	QString m_LocalFix;
	QString m_UsbFix;
	QString m_CdFix;
};

class BGIBASE_CLASS_EXPORT BgiDiskTools
{
public:
	static int GetFreeDiskSpace(const QString& driver);
	static int GetFreeDiskSpace(EDriveLetter dirver);
	static void GetDiskSpaceInfo(const QString& driver, float& total, float& free);
	static float ConvertBtoGb(double space);
	static void GetCurrentSysDiskName(QMap<QString, EDiskType>& map);
	static bool IsCD(const QString& diskRoot);
private:
	static void ReturnFreeDiskSpace(const QString& driver, float& total, float& free);

#ifdef WIN32
	static INT GetBusType(TCHAR driver);
#endif // WIN32
};


#endif