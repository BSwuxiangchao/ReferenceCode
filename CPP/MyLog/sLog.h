#pragma once
#ifndef SSLOG_DEH
#define SSLOG_DEH

#include"MyLog.h"

DLL_EXPORT_API void InitLog(std::string file_name, int log_level = LEVEL_TRACE, TERMINAL_COLOR color = COLOR_AUTOMATIC);
DLL_EXPORT_API void InitFiLog(std::string file_name, int log_level = LEVEL_TRACE);


#endif // SSLOG_DEH
