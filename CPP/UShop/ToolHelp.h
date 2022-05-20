//copyright 2021 dragon@oct

//辅助工具，函数
/* $Header:PlcServer/ToolHelpS.h              5/28/2021 16:44              */
/**************************************************************************
***                                                                     ***
*                                                                         *
*                    Project Name: PlcServer                              *
*                                                                         *
*                       File Name: ToolHelpS.h                             *
*                                                                         *
*                      Start Date: 5/28/2021                              *
*                                                                         *
*                       Last Date: 5/28/2021                              *
*-------------------------------------------------------------------------*
* Functions:                                                              *
*-------------------------------------------------------------------------*
**************************************************************************/

#pragma once
#include<string>
#include<sstream>
#include<cstdlib>
#include <io.h>
#include<vector>
#include<time.h>

namespace ToolHelp
{
	//十六进制输出字符串
	std::string GetHexString(const char* s, int len);
	//获取0-100之间的随机数,以当前时间为索引
	int GetRandNumberS();
	//获取0-100之间的随机数
	int GetRandNumber();
	void GetFiles(std::string path, std::vector<struct _finddata_t>& files);
}