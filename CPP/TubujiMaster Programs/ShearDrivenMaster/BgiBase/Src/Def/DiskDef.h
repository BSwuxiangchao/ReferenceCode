/*
* @Abrief 硬盘定义
* @note 硬盘类型、盘符宏定义
* @version 1.0
* @author mosongjin 2021-07-21
*/

#ifndef _BGI_DISK_DEF_H
#define _BGI_DISK_DEF_H


#include <QObject>
#include <QString>
#include "ExternDef.h"

enum BGIBASE_CLASS_EXPORT EDiskType
{
	type_local,
	type_remove,
	type_cd
};
Q_DECLARE_METATYPE(EDiskType);

enum BGIBASE_CLASS_EXPORT EDriveLetter
{
	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K
};
Q_DECLARE_METATYPE(EDriveLetter);





#endif