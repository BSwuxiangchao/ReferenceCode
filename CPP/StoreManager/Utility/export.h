#pragma once

#ifdef DLL_API//如果已经定义就什么都不做
     //nothing to do
 #else //否则定义DLL_API
     #define DLL_API __declspec(dllexport)    //_declspec(dllexport)：导出标志
 #endif
