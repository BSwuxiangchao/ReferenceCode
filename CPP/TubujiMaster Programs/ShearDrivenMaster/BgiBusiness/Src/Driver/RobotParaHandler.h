/*
* @Abrief 调试过程中的参数修改
* @note 调试过程中的参数输出
* @version 1.0
* @author wuxiangchao 2023-02-27
*/

#ifndef _PROG_CONTEXT_H
#define _PROG_CONTEXT_H

#include <qobject.h>
#include <qvariant.h>
#include "BgiProcId.h"

class EpsonRobotEventHandler;
class RobotParaHandler : public QObject
{
	Q_OBJECT
public:
	RobotParaHandler();
	virtual ~RobotParaHandler();

	void SetEpsonRobotEventHandler(EpsonRobotEventHandler* handler);
public slots:
	void ReceiveRobotParaDesc(QVariant desc);

private:
};

#endif

