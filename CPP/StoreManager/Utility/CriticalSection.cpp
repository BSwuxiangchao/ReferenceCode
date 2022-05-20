//author: sang-anlei
//date:2019/1/10

//#include "stdafx.h"
#include "CriticalSection.h"


Utils::CritSec::CritSec(void)
{ 
	InitializeCriticalSection( &cs );	
}

Utils::CritSec::~CritSec(void)
{ 
	DeleteCriticalSection( &cs );	
}

void Utils::CritSec::Lock(void)
{ 
	EnterCriticalSection( &cs );	
}

void Utils::CritSec::Unlock(void)
{ 
	LeaveCriticalSection( &cs );	
}
