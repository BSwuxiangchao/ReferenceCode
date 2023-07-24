/*
* @Abrief 涂布机共用的设备接口
* @note 共用设备接口
* @version 1.0
* @author wuxiangchao 2023-2-7
*/

#ifndef _MASTER_DEVICES_H_
#define _MASTER_DEVICES_H_

#include <qobject.h>
#include <qsharedpointer.h>
#include <qmap.h>
#include "BusinessExternDef.h"
#include "BgiLock.h"

class EpsonRobotDriver;
class BgiTcpSocket;
class CoaterDriver;
class ImagerDriver;
class SchedulerDriver;
class ScannerDriver;

class BGIBUSINE_CLASS_EXPORT MasterDevices : public QObject
{
    Q_OBJECT
public:
    ~MasterDevices();

    static MasterDevices* Instance();

    EpsonRobotDriver* GetEpsonRobotDriver();
    BgiTcpSocket* GetRobotSocket();

    CoaterDriver* GetCoaterDriver();

    ImagerDriver* GetImagerDriver();

    SchedulerDriver* GetSchedulerDriver();
    
    ScannerDriver* GetScannerDriver();
private:
    MasterDevices();

    BgiTcpSocket* GetCoaterSocket();

private:
    QSharedPointer<BgiTcpSocket> m_spRobotSocket;
    QSharedPointer<EpsonRobotDriver> m_spEpsonRobotDriver;

    QSharedPointer<BgiTcpSocket> m_spCoaterSocket;
    QSharedPointer<CoaterDriver> m_spCoaterDriver;

    QSharedPointer<ImagerDriver> m_spImagerDriver;

    QSharedPointer<SchedulerDriver> m_spSchedulerDriver;

    QSharedPointer<BgiTcpSocket> m_spScannerSocket;
    QSharedPointer<ScannerDriver> m_spScannerDriver;

    BgiLock m_InitLock;
};

#endif