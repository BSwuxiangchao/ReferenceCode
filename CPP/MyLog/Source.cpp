#include"sLog.h"
int main(int argc, char* argv[])
{
	char _path[256] = { 0 };
	_getcwd(_path, 256);
	strcat(_path, R"(\log2.txt)");
	InitLog(_path);
	char psth[] = "Œ“∞Æ∫˙√®√®";
	SZL_WARN("sdsfsd :{0}£¨{1}", psth,"∂‡«È◊‘π≈ø’”‡∫ﬁ");
	SZL_DEBUG("sdsfsd :{0}", psth);
	SZL_TRACE("sdsfsd :{0}", psth);
	SZL_ERROR("sdsfsd :{0}", psth);
	SZL_FATAL("sdsfsd :{0}", psth);
	SZL_INFO("sdsfsd :{0}", psth);
	system("pause");
	return 0;
}