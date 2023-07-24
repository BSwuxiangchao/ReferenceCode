/*
* @Abrief 输出参数处理类
* @note 设置、修改、查询参数的帮助函数
* @version 1.0
* @author wuxiangchao 2023-2-27
*/
#ifndef _ROBOT_OUTPUT_PARA_UTIL_H_
#define _ROBOT_OUTPUT_PARA_UTIL_H_

#include "ParaType.h"
#include <vector>
#include "BgiProcId.h"
#include "RobotParaType.h"
#include <QMutex>
#include "BusinessExternDef.h"

const QString gc_RescanTip = "Rescan Code Please!!!";
const QString gc_ScanErrTip = "Scan Code failed,Check Please!!!";

class BGIBUSINE_CLASS_EXPORT RobotOutputParaUtil
{
public:
    RobotOutputParaUtil();
 
    static void SetRobotPos(RobotParaType::RobotPoint value);
    static void SetRobotStatus(RobotParaType::RobotStatus value);
    static void SetRobotPara(RobotParaType::RobotPara value);
    static void SetRobotSafetyState(int iState);
    static int GetRobotSafetyState();
    static void SetScannerInfo(QString strInfo);
    static bool GetScannerInfo(QString &strInfo);

private:
    static void ApplicationSleep(int mSec);

private:

    // 绑定合成序列信息
    static QVector<RobotParaType::RobotPara> m_vecRobotParaStatisSeq;
    static int m_iAtRobotSafety;
    // 扫码器信息
    static QString m_strScannerInfo;
};


#endif
