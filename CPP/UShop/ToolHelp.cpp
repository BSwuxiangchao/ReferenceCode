#include"stdafx.h"
#include"ToolHelp.h"
namespace ToolHelp
{
	//ʮ����������ַ���
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
	//��ȡ0-100֮��������,�Ե�ǰʱ��Ϊ����
	int GetRandNumberS()
	{
		time_t ttm = time(nullptr);
		srand((int)ttm);
		return rand() % 100;
	}
	//��ȡ0-100֮��������
	int GetRandNumber()
	{
		time_t ttm = time(nullptr);
		ttm++;
		srand((int)ttm);
		return rand() % 100;
	}
	//��ȡ�ļ����������ļ�(�������ļ����µ��ļ�),  \*,\*.log,\*.text,..
	void GetFiles(std::string path, std::vector<struct _finddata_t>& files)
	{
		//�ļ����
		intptr_t    hFile = 0;
		//�ļ���Ϣ
		struct _finddata_t fileinfo;
		std::string dirP, p;
		dirP.assign(path.substr(0, path.find_last_of('\\')));

		if ((hFile = _findfirst(p.assign(path).c_str(), &fileinfo)) != -1)
		{
			do
			{
				//�����Ŀ¼,����֮
				//�������,�����б�
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