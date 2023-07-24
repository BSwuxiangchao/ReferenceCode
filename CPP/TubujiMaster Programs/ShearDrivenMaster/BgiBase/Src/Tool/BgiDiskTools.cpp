#include "BgiDiskTools.h"

#include <QDir>
#include <QFile>
#include <iostream>
#include <string>

using namespace std;

void BgiDiskTools::GetCurrentSysDiskName(QMap<QString, EDiskType>& map)
{
	DWORD dwLen = GetLogicalDriveStrings(0, NULL);	//获取系统字符串长度.		//构建一个相应长度的数组.
	WCHAR* ws = new WCHAR[dwLen];
	WCHAR* temp = ws;
	memset(ws, 0, sizeof(ws));
	/*MultiByteToWideChar(CP_ACP, 0, pszDriver, strlen(pszDriver) + 1, ws,
		sizeof(ws) / sizeof(ws[0]));*/


	GetLogicalDriveStrings(dwLen, ws);		//获取盘符字符串.
	while (*ws != '\0')
	{
		TCHAR tszDriver[4] = { TEXT("*:\\") };
		tszDriver[0] = ws[0];
		UINT TYPE = GetDriveType(tszDriver);
		switch (TYPE)
		{
		case DRIVE_FIXED://本地磁盘
#ifdef QT_NO_DEBUG
			if (*ws == 'D' || *ws == 'd')
			{
				map[QString(*ws) + ":"] = EDiskType::type_local;
			}
#else
			map[QString(*ws) + ":"] = EDiskType::type_local;
#endif // DEBUG
			break;
		case DRIVE_REMOVABLE://可移动磁盘
			map[QString(*ws) + ":"] = EDiskType::type_remove;
			break;
		case DRIVE_CDROM://CD光驱
		{

			map[QString(*ws) + ":"] = EDiskType::type_cd;
			break;
		}
		case DRIVE_UNKNOWN://未知磁盘
		case DRIVE_NO_ROOT_DIR://无效名称
		case DRIVE_REMOTE://网络磁盘
		case DRIVE_RAMDISK://RAM
			break;
		default:
			break;
		}

		ws += sizeof(ws) / sizeof(*ws);
		//delete pszOut;
	}
	delete temp;
}

bool BgiDiskTools::IsCD(const QString& diskRoot)
{
	TCHAR tszDriver[4] = { TEXT("*:\\") };
	tszDriver[0] = diskRoot.toStdString()[0];
	UINT TYPE = GetDriveType(tszDriver);

	if (TYPE == DRIVE_CDROM)
	{
		return true;
	}
	return false;
}

INT BgiDiskTools::GetBusType(TCHAR driver)
{
	bool bResult = true;
	INT BusType = BusTypeUnknown;//BusTypeUsb
	HANDLE hDrv = INVALID_HANDLE_VALUE;
	STORAGE_DEVICE_DESCRIPTOR DevDesc;
	STORAGE_PROPERTY_QUERY   ProQuery;

	TCHAR tszSymbol[MAX_PATH] = { TEXT("\\\\?\\*:") };
	ULONG ulBytesResults = 0;
	memset(&ProQuery, 0, sizeof(ProQuery));
	memset(&DevDesc, 0, sizeof(DevDesc));

	if ((driver >= TEXT('A') && driver <= TEXT('Z')) || (driver >= TEXT('a') && driver <= TEXT('z')))
	{

	}
	else
	{
		return false;
	}

	tszSymbol[4] = driver;

	hDrv = CreateFile(tszSymbol, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hDrv == INVALID_HANDLE_VALUE)
		goto jump_ret;

	ProQuery.QueryType = PropertyStandardQuery;
	ProQuery.PropertyId = StorageDeviceProperty;
	if (DeviceIoControl(hDrv, IOCTL_STORAGE_QUERY_PROPERTY, &ProQuery, sizeof(ProQuery),
		&DevDesc, sizeof(DevDesc), &ulBytesResults, NULL) == FALSE)
		goto jump_ret;

	BusType = DevDesc.BusType;

jump_ret:
	if (hDrv != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hDrv);
		hDrv = INVALID_HANDLE_VALUE;
	}
	return BusType;
}

int BgiDiskTools::GetFreeDiskSpace(const QString& drivername)
{
	QString driver = drivername.toUpper();
	driver = driver + (driver.contains(":") ? "/" : ":/");

	float free = 0;
	float total = 0;
	ReturnFreeDiskSpace(driver, total, free);
	return free;
}

int BgiDiskTools::GetFreeDiskSpace(EDriveLetter dirver)
{
	QString diskname = "";
	switch (dirver)
	{
	case A:
		diskname = "A:/";
		break;
	case B:
		diskname = "B:/";
		break;
	case C:
		diskname = "C:/";
		break;
	case D:
		diskname = "D:/";
		break;
	case E:
		diskname = "E:/";
		break;
	case F:
		diskname = "F:/";
		break;
	case G:
		diskname = "G:/";
		break;
	case H:
		diskname = "H:/";
		break;
	case I:
		diskname = "I:/";
		break;
	case J:
		diskname = "J:/";
		break;
	case K:
		diskname = "K:/";
		break;
	default:
		break;
	}

	float free = 0;
	float total = 0;
	ReturnFreeDiskSpace(diskname, total, free);
	return free;
}

void BgiDiskTools::GetDiskSpaceInfo(const QString& driver, float& total, float& free)
{
	QString diskname = driver.toUpper();
	diskname = driver + (driver.contains(":") ? "/" : ":/");
	if (IsCD(driver))
	{
		//CD工具获取内容大小，暂不提供2021-07-21
	}
	else
	{
		ReturnFreeDiskSpace(diskname, total, free);
	}
}

void BgiDiskTools::ReturnFreeDiskSpace(const QString& driver, float& total, float& free)
{
	LPCWSTR lpcwstrDriver = (LPCWSTR)driver.utf16();
	ULARGE_INTEGER liFreeBytesAvailable, liTotalBytes, liTotalFreeBytes;
	if (!GetDiskFreeSpaceEx(lpcwstrDriver, &liFreeBytesAvailable, &liTotalBytes, &liTotalFreeBytes))
	{
		return;
	}
	free = ConvertBtoGb((double)liTotalFreeBytes.QuadPart);
	total = ConvertBtoGb((double)liTotalBytes.QuadPart);
}

float BgiDiskTools::ConvertBtoGb(double space)
{
	return space / 1024 / 1024 / 1024;
}