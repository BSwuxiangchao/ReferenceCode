/*
* @Abrief
* @note Master相关宏定义
* @version 1.0
* @author wuxiangchao 2023-03-06
*/
#pragma once
#include "qstring.h"
#include <QStringList>
#include "TransformCoding.h"
enum ChangeType
{
    toChinese,      //中文
    DtoForeign,     //从中文直接转到其他
    InDtoForeign,   //从其他转到其他（通过中文中转）
};
const QString gc_Chinese = "Chinese";
const QString gc_English = "English";
const QString gc_French = "French";

const QStringList gc_lstSequencingTableHeader = { StringConversionEncoding("序号"), StringConversionEncoding("芯片号"),StringConversionEncoding("光机编号"), StringConversionEncoding("测序模式")
, StringConversionEncoding("生化次数") , StringConversionEncoding("拍照次数"), StringConversionEncoding("R1"), StringConversionEncoding("R2"), StringConversionEncoding("B1"), StringConversionEncoding("B2")
, StringConversionEncoding("ALL")};

const QStringList gc_lstErrLogTableHeader = { StringConversionEncoding("时间"),StringConversionEncoding("类型"),StringConversionEncoding("信息") };

const QStringList gc_lstBasecallMemTableHeader = { 
    StringConversionEncoding("SlideID"),
    StringConversionEncoding("CurCycle"),
    StringConversionEncoding("StarRow"),
    StringConversionEncoding("EndRow"),
    StringConversionEncoding("StartCol"),
    StringConversionEncoding("EndCol"),
    StringConversionEncoding("StartCycle"),
    StringConversionEncoding("Read1Len"),
    StringConversionEncoding("Read2Len"),
    StringConversionEncoding("TotalCycle")};
