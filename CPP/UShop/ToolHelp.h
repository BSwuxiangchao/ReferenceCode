//copyright 2021 dragon@oct

//�������ߣ�����
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
	//ʮ����������ַ���
	std::string GetHexString(const char* s, int len);
	//��ȡ0-100֮��������,�Ե�ǰʱ��Ϊ����
	int GetRandNumberS();
	//��ȡ0-100֮��������
	int GetRandNumber();
	void GetFiles(std::string path, std::vector<struct _finddata_t>& files);
}