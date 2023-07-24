/*
* @Abrief 
* @note 主控处理流入的事件
* @version 1.0
* @author wuxiangchao 2023-02-20
*/

#ifndef _BGI_MPS_APP_H_
#define _BGI_MPS_APP_H_

#include <qobject.h>
#include <qthread.h>

class DriveHandlerContext;

class BgiMasterApp : public QObject
{
    Q_OBJECT
public:
    ~BgiMasterApp();

    static BgiMasterApp* Instance();
    bool event(QEvent* pEvent);
    bool eventFilter(QObject* obj, QEvent* pEvent);

    void InstallEventEventFilter(QObject* obj);

private:
    BgiMasterApp();

public slots:

private:
    QThread* m_pThread;
    //ProgContext* m_pProgContext;
    DriveHandlerContext* m_pDriveHandlerContext;
};


#endif //_BGI_MPS_APP_H_
