#pragma once

#ifdef DLL_API//����Ѿ������ʲô������
     //nothing to do
 #else //������DLL_API
     #define DLL_API __declspec(dllexport)    //_declspec(dllexport)��������־
 #endif
