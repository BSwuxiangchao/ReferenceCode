#include"sLog.h"

void InitLog(std::string file_name, int log_level, TERMINAL_COLOR color)
{
	MyLog::Instance()->InitFileMyLog(file_name, log_level);
	MyLog::Instance()->InitTerminalMyLog(color);
}
void InitFiLog(std::string file_name, int log_level)
{
	MyLog::Instance()->InitFileMyLog(file_name, log_level);
}