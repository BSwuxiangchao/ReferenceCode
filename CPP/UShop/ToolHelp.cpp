#include"stdafx.h"
#include"ToolHelp.h"
namespace ToolHelp
{
	//十六进制输出字符串
	std::string GetHexString(const char* s, int len)
	{
		std::ostringstream out;
		out << '\"';
		out << std::hex;
		for (int n = 0; n < len; n++)
		{
			out << " 0x" << (static_cast<short>(*s++) & 0xff);
		}
		out << '\"';
		return out.str();
	}
	//获取0-100之间的随机数,以当前时间为索引
	int GetRandNumberS()
	{
		time_t ttm = time(nullptr);
		srand((int)ttm);
		return rand() % 100;
	}
	//获取0-100之间的随机数
	int GetRandNumber()
	{
		time_t ttm = time(nullptr);
		ttm++;
		srand((int)ttm);
		return rand() % 100;
	}
	//获取文件夹下所有文件(包括子文件夹下的文件),  \*,\*.log,\*.text,..
	void GetFiles(std::string path, std::vector<struct _finddata_t>& files)
	{
		//文件句柄
		intptr_t    hFile = 0;
		//文件信息
		struct _finddata_t fileinfo;
		std::string dirP, p;
		dirP.assign(path.substr(0, path.find_last_of('\\')));

		if ((hFile = _findfirst(p.assign(path).c_str(), &fileinfo)) != -1)
		{
			do
			{
				//如果是目录,迭代之
				//如果不是,加入列表
				if ((fileinfo.attrib & _A_SUBDIR))
				{
					if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
						GetFiles(p.assign(path).append("\\").append(fileinfo.name), files);
				}
				else
				{
					memcpy(fileinfo.name, (dirP + "\\" + fileinfo.name).c_str(), sizeof(fileinfo.name));
					files.emplace_back(fileinfo);
				}
			} while (_findnext(hFile, &fileinfo) == 0);
			_findclose(hFile);
		}
	}
}