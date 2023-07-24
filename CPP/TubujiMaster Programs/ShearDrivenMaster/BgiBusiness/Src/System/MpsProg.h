/*
* @Abrief 合成软件具有的功能方案
* @note 功能方案的基类：单分拣，双分拣，单合成，双合成，单分拣合成，双分拣合成，单精分
* @version 1.0
* @author mosongjin 2021-08-16
*/

#ifndef _MPS_PROG_H
#define _MPS_PROG_H

#include "BgiProcId.h"

class MpsProg 
{
public:
	MpsProg() {};
	virtual ~MpsProg() {};

	virtual void Create() = 0;
	virtual void Init() = 0;
	virtual void Start() = 0;
	virtual void Stop() = 0;
	virtual void Begin() = 0;
	virtual void End() = 0;
	//virtual BgiProc::ESystemType GetSystemType() = 0;
	virtual QString GetMpsProgName() = 0;
	virtual void Done(BgiProc::ESystemId /*systemId*/) {};
	virtual void Exit(BgiProc::ESystemId /*systemId*/) {};
	virtual void Destroy() {};

private:

};

#endif

