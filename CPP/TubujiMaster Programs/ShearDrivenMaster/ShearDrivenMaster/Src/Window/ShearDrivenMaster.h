#pragma once

#include <QtWidgets/QMainWindow>
#include <QGroupBox>
#include <QCloseEvent>
#include "ui_ShearDrivenMaster.h"
#include "ImagerWidget.h"
#include "RobotWidget.h"
#include "CoaterWidget.h"
#include "MessageWidget.h"
#include "SequencingWidget.h"
#include "ProcessOperatePromptInfoThread.h"
#include "MasterDefine.h"
#include "ParaType.h"

class EpsonRobotDebug;
class ProcessOperatePromptInfoThread;
class InfoPromptWidget;
class RobotConnectParaControl;
class CLanguageMgr;
class ImagerMgrWidget;
class MainTitleWidget;
class SchedulerDriver;
class LocalDataWidgeet;
class ErrorLogWidget;
class CoaterMgrWidget;

class ShearDrivenMaster : public QMainWindow
{
    Q_OBJECT

public:
    ShearDrivenMaster(QWidget *parent = nullptr);
    ~ShearDrivenMaster();
    void closeEvent(QCloseEvent*);

    void SetUserInfo(const GeneralParaType::UserInfo userInfo);

signals:
    void SignalChangeAboutButtonText(QString strText);
    void SignalBacktoLoginDlg();

public slots:
    // 本地数据
    void on_ChipSeting_Clicked();
    // 机械臂设置
    void On_Debug_Clicked();
    // 光机设置
    void On_ImagerSeting_Clicked();
    // 涂布机设置
    void On_CoaterSetting_Clicked();
    // 系统初始化
    void On_SystemInit_Clicked();
    // 报警重置
    void On_WarningReset_Clicked();
    // 一键拍照
    void On_OneKeyPhoto_Clieked();
    // 错误信息
    void On_ErrorInfo_Clicked();
    // 接受弹窗信息
    void SlotOpePromptInfo(_tagPromptData data);
    // 右键弹框槽函数
    void on_MainWindow_customContextMenuRequested(const QPoint& pos);
    void on_Language1_TriggerdSlot();
    void on_Language2_TriggerdSlot();
    void on_Language3_TriggerdSlot();
    void on_Version_TriggerdSlot();
    void on_quitLogin_TriggerdSlot();
    void on_quit_TriggerdSlot();

    void On_StartRun_Clicked();
    void On_PauseRun_Clieked();
    void On_StopRun_Clicked();
    void On_SetCycle_Clicked();

    void SetStopRunState();
    void SetRobotWidgetCtrl(QGroupBox* qRobotGroupBox, QGroupBox* qRobotParaGroupBox);

private:
    void TestOwnerCode();
    void InitWidget();
    // 设置样式
    void SetStyleSheet();
    bool CloseMainwindowUnit();
    // 添加工具栏
    void CreateToolBar();
    void ChangeUIControlText(ChangeType type);
    void on_MainWindow_customContextMenuRequested_unit();
    // 发送事件
    void PostRobotEvent(QString strActionName);

    void StopSchedulerTask();

private:
    Ui::MainWindow ui;
    QToolBar* m_mainToolBar;

    bool m_bAlreadyCloseWindow;
    QSharedPointer<ImagerWidget> m_pImagerWidget;
    QSharedPointer<RobotWidget> m_pRobotWidget;
    QSharedPointer<CoaterWidget> m_pCoaterWidget;
    QSharedPointer<MessageWidget> m_pMessageWidget;
    QSharedPointer<SequencingWidget> m_pSequencingWidget;

    QSharedPointer<EpsonRobotDebug> m_Debugdlg;
    QSharedPointer<LocalDataWidgeet> m_pLocalDataWidget;
    QSharedPointer<ImagerMgrWidget> m_pImagerMgrWidget;
    QSharedPointer<CoaterMgrWidget> m_pCoaterMgrWidget;
    QSharedPointer< ErrorLogWidget> m_pErrorLogWidget;

    SchedulerDriver* m_pSchedulerDriver;

    // 弹窗线程
    ProcessOperatePromptInfoThread* m_pProcessOpePromptInfoThread;
    // 提示框
    InfoPromptWidget* m_pInfoPrompt;
    bool m_bShowTwobtn;
    RecordData m_strData;
    //robot连接监控
    RobotConnectParaControl* m_pRobotConnectParaControl;
    //多语言
    ChangeType m_ChangeType;
    CLanguageMgr *m_CLanguageMgr;
    QString m_strCurLanguage;
    //添加Logo
    QHBoxLayout* m_pLayoutMainTitle;
    QSharedPointer<MainTitleWidget> m_spMainTitleWidget;

    //用户信息
    GeneralParaType::UserInfo m_UserINfo;
    QGroupBox* m_qRobotGroupBox;
    QGroupBox* m_qRobotParaGroupBox;

};
