/*
* @Abrief 常规工具类
* @note 一些简便的工具，减少代码编写
* @version 1.0
* @author mosongjin 2021-07-21
*/

#ifndef _BGI_GENERAL_TOOL_H
#define _BGI_GENERAL_TOOL_H

#include <stdio.h>
#include <math.h>

#include "ExternDef.h"

#define RELEASE_ARRAY(p) if(NULL!=p){delete[] p;p=NULL;}
#define RELEASE_POINTER(p) if(NULL!=p){delete p;p=NULL;}

#define NULL 0

#define FLOAT_EQUAL(a, b)           ( (a) <= (b) + 1E-6 && (a) >= (b) - 1E-6 )
#define FLOAT_NOT_EQUAL(a, b)       ( (a) > (b) + 1E-6 || (a) < (b) - 1E-6 )

#define FLOAT_GREATER(a, b)   ( (a) > (b) + 1e-6 )
#define FLOAT_GREATER_EQUAL(a, b)   ( (a) > (b) - 1e-6 )

#define FLOAT_LESS(a, b)   ( (a) < (b) - 1e-6 )
#define FLOAT_LESS_EQUAL(a, b)      ( (a) < (b) + 1e-6 )

#define MAKE_WORD(high, low)        (((static_cast<unsigned short>(high)<<8) & 0xFF00) | (static_cast<unsigned short>(low) & 0x00FF))
#define MAKE_DWORD(high, low)       (((static_cast<unsigned int>(high)<<16) & 0xFFFF0000) | (static_cast<unsigned int>(low) & 0x0000FFFF))
#define MAKE_QWORD(high, low)       (((static_cast<unsigned long long>(high)<<32) & 0xFFFFFFFF00000000) | (static_cast<unsigned long long>(low) & 0x00000000FFFFFFFF))

#define LOW8(v)      ((char)(((unsigned short)v) & 0x00FF)) 
#define HIGH8(v)      ((char)((((unsigned short)v) >> 8) & 0x00FF))
#define LOW16(v)   ((short)(((unsigned int)v) & 0xFFFF))
#define HIGH16(v)   ((short)((((unsigned int) v) >> 16) & 0xFFFF))
#define LOW32(v)   ((unsigned int)(((long long)v) & 0x00000000FFFFFFFF))
#define HIGH32(v)   ((unsigned int)((((long long) v) >> 32) & 0x00000000FFFFFFFF))


inline bool BGIBASE_CAPI_EXPORT Equal_float(float a, float b)
{
    static const float FLT_DELTA = 0.00001f;
    return (fabsf(a - b) < FLT_DELTA);
}



#endif