/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 创建时间：2022/7/20 10:03
 * 功能描述：合成模块常用名称
 ************************************* end-XCW *****************************************/
#pragma once
#include <QString>
#include <QStringList>
#include "TransformCoding.h"


// 注液表表头名称
const QStringList gc_lstInjectionTableHeader = { StringConversionEncoding("序号"), StringConversionEncoding("注液表文件名称"), StringConversionEncoding("开始"), StringConversionEncoding("结束")};
const QStringList gc_lstInjectionTableContentHeader = { StringConversionEncoding("事件顺序"), StringConversionEncoding("注液试剂"), StringConversionEncoding("试剂量系数"),
											StringConversionEncoding("反应时间(s)"), StringConversionEncoding("管路消耗(ml)") };

/* 合成统计表头内容：芯片编码，序列编号，序列名称，合成进展，合成序列 */
const QStringList gc_lstSynthsisProgTableHeader = { StringConversionEncoding("芯片编码"), StringConversionEncoding("序列编号"), StringConversionEncoding("序列名称"),
									 StringConversionEncoding("合成进展"), StringConversionEncoding("合成序列"), StringConversionEncoding("合成状态") };

// 合成序列状态
const QString gc_strSeqSynthsisStateDone = "完成";
const QString gc_strSeqSynthsisStateNotDone = "合成中";

// 合成信息文件夹
const QString gc_strFolderSynthesisModule = "SynthesisModule";
const QString gc_strFolderSeqInfoOutput = "SeqInfoOutput";




