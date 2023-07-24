/*
* @Abrief 导出导入定义
* @note 外部导入导出的宏定义
* @version 1.0
* @author mosongjin 2021-07-15
*/

#ifndef _BGI_EXTERN_DEF_H
#define _BGI_EXTERN_DEF_H


#pragma warning (disable : 4786)
#pragma warning (disable : 4231)
#pragma warning (disable : 4819)


#ifdef BGIBUILDDLL

#ifdef BGIBASEEXTERNEXPORT

#define BGIBASE_API_EXPORT extern "C" __declspec(dllexport)
#define BGIBASE_VARIANT_EXPORT extern "C" __declspec(dllexport)
#define BGIBASE_CLASS_EXPORT __declspec(dllexport)
#define BGIBASE_CAPI_EXPORT __declspec(dllexport)
#define BGIBASE_TEMPLATE_EXPORT


#else
#define BGIBASE_API_EXPORT extern "C" __declspec(dllimport)
#define BGIBASE_VARIANT_EXPORT extern "C" __declspec(dllimport)
#define BGIBASE_CLASS_EXPORT __declspec(dllimport)
#define BGIBASE_CAPI_EXPORT __declspec(dllimport)
#define BGIBASE_TEMPLATE_EXPORT 
#endif

#else
#define BGIBASE_API_EXPORT 
#define BGIBASE_VARIANT_EXPORT 
#define BGIBASE_CLASS_EXPORT 
#define BGIBASE_CAPI_EXPORT 
#define BGIBASE_TEMPLATE_EXPORT 
#endif



#endif