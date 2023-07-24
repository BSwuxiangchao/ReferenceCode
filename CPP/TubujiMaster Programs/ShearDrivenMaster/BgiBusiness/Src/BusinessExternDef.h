/*
* @Abrief 导出导入定义
* @note 外部导入导出的宏定义
* @version 1.0
* @author mosongjin 2021-08-17
*/

#ifndef _BGI_BUSINESS_EXTERN_DEF_H
#define _BGI_BUSINESS_EXTERN_DEF_H


#pragma warning (disable : 4786)
#pragma warning (disable : 4231)
#pragma warning (disable : 4819)


#ifdef BGIBUILDDLL

#ifdef BGIBUSINESSEXTERNEXPORT

#define BGIBUSINE_API_EXPORT extern "C" __declspec(dllexport)
#define BGIBUSINE_VARIANT_EXPORT extern "C" __declspec(dllexport)
#define BGIBUSINE_CLASS_EXPORT __declspec(dllexport)
#define BGIBUSINE_CAPI_EXPORT __declspec(dllexport)
#define BGIBUSINE_TEMPLATE_EXPORT


#else
#define BGIBUSINE_API_EXPORT extern "C" __declspec(dllimport)
#define BGIBUSINE_VARIANT_EXPORT extern "C" __declspec(dllimport)
#define BGIBUSINE_CLASS_EXPORT __declspec(dllimport)
#define BGIBUSINE_CAPI_EXPORT __declspec(dllimport)
#define BGIBUSINE_TEMPLATE_EXPORT 
#endif

#else
#define BGIBUSINE_API_EXPORT 
#define BGIBUSINE_VARIANT_EXPORT 
#define BGIBUSINE_CLASS_EXPORT 
#define BGIBUSINE_CAPI_EXPORT 
#define BGIBUSINE_TEMPLATE_EXPORT 
#endif



#endif