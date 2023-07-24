/*
* @Abrief 参数事件基类
* @note 用于子线程与主线程间的参数传递
* @version 1.0
* @author mosongjin 2021-08-17
*/

#ifndef _PARA_EVENT_H
#define _PARA_EVENT_H

#include <qobject.h>

class ParaEngine;
class SystemModel;

class ParaEvent : public QObject
{
public:
	ParaEvent(ParaEngine* paraEngine) 
		:m_pParaEngine(paraEngine)
		, m_pSystemModel(NULL)
	{
	};

	virtual ~ParaEvent() {};

	virtual void SetSystemModel(SystemModel* systemModel) 
	{
		m_pSystemModel = systemModel;
	};

protected:
	ParaEngine* m_pParaEngine;
	SystemModel* m_pSystemModel;

};

#endif

