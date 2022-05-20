
#ifndef __CRITICAL_SECTION__H
#define __CRITICAL_SECTION__H
#include <windows.h>

namespace Utils {

	class CritSec
	{
		CRITICAL_SECTION cs;
	public:
		CritSec(void);

		~CritSec(void);

		void Lock(void);

		void Unlock(void);
	};
}



#endif