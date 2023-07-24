#include "ShearDrivenMaster.h"
#include "BgiLogger.h"
#include "MasterDevices.h"
#include "EpsonRobotDebug.h"
#include "EpsonRobotFlowCtrl.h"
#include "TransformCoding.h"
#include "ProcessOperatePromptInfoThread.h"
#include "InfoPromptWidget.h"
#include "OpeStyleSheet.h"
#include "RobotConnectParaControl.h"
#include "languagemgr.h"
#include <QRegularExpressionValidator>
#include "ParaType.h"
#include "ImagerMgrWidget.h"
#include "ErrorCodeTranslate.h"
#include "MainTitleWidget.h"
#include "BgiMpsEvent.h"
#include "BgiMasterApp.h"
#include <QMessageBox.h>
#include "SchedulerDriver.h"
#include "OperateSqliteConfig.h"
#include "LocalDataWidgeet.h"
#include "ErrorLogWidget.h"
#include "RobotOutputParaUtil.h"
#include "CoaterMgrWidget.h"
#include "ImagerDriver.h"
#include "BGIIputDialog.h"


ShearDrivenMaster::ShearDrivenMaster(QWidget *parent)
    : QMainWindow(parent)
    , m_bAlreadyCloseWindow(false)
    , m_pProcessOpePromptInfoThread(NULL)
    , m_pInfoPrompt(NULL)
    , m_bShowTwobtn(false)
    , m_ChangeType(toChinese)
    , m_strCurLanguage(gc_Chinese)
    , m_qRobotGroupBox(nullptr)
    , m_qRobotParaGroupBox(nullptr)
{
    //ErrorCodeTranslate::Instance();
    ui.setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    m_CLanguageMgr = CLanguageMgr::Instance();

    InitWidget();
    //设置样式表
    SetStyleSheet();
    //CreateToolBar();
    //消息弹框
    m_pInfoPrompt = new InfoPromptWidget((QDialog*)this);

    // 弹窗线程
    m_pProcessOpePromptInfoThread = new ProcessOperatePromptInfoThread();
    connect(m_pProcessOpePromptInfoThread, &ProcessOperatePromptInfoThread::SendRecordData, this, &ShearDrivenMaster::SlotOpePromptInfo);
    m_pProcessOpePromptInfoThread->StartThread();

    BGI_MESSAGE("程序已正常启动");
    m_strData.m_strInfoType = "温馨提示";
    m_strData.m_strContent = "程序已正常启动！";
    m_strData.m_color = gc_colorBlack;
    OperateRecordInfo::AddRecordData(m_strData);

    
    //发送事件处理
    /*BgiMpsEvent* p_event = NULL;
    p_event = new BgiMpsEvent("Reset_All_Device_Event");
    p_event->SetAction("Reset_Synthsis_Device");
    BgiMpsApp* app = BgiMpsApp::Instance();
    QApplication::postEvent(app, p_event);*/
}

ShearDrivenMaster::~ShearDrivenMaster()
{
    if (NULL != m_pInfoPrompt)
    {
        delete m_pInfoPrompt;
        m_pInfoPrompt = NULL;
    }
    if (NULL != m_pProcessOpePromptInfoThread)
    {
        m_pProcessOpePromptInfoThread->StopThread();
        delete m_pProcessOpePromptInfoThread;
        m_pProcessOpePromptInfoThread = NULL;
    }
}

void ShearDrivenMaster::SetUserInfo(const GeneralParaType::UserInfo userInfo)
{
    m_UserINfo = userInfo;
    //根据用户类型显示控件
    ui.ButtonChipSeting->setEnabled(true);
    ui.ButtonImagerSeting->setEnabled(true);
    ui.ButtonRobotSeting->setEnabled(true);
    ui.ButtonCoaterSeting->setEnabled(true);
    ui.ButtonSystemInit->setEnabled(true);
    ui.ButtonAlarmReset->setEnabled(true);
    ui.ButtonTakePhotos->setEnabled(true);
    ui.ButtonQueryLogs->setEnabled(true);
    if (userInfo.userType == GeneralParaType::UserType::Operator)
    {
        ui.ButtonImagerSeting->setEnabled(false);
        ui.ButtonRobotSeting->setEnabled(false);
        ui.ButtonCoaterSeting->setEnabled(false);
    }
    else if(userInfo.userType == GeneralParaType::UserType::Maintainer)
    {
        ui.ButtonRobotSeting->setEnabled(false);
        ui.ButtonCoaterSeting->setEnabled(false);
    }
    m_pRobotConnectParaControl->SetUserInfo(userInfo);
    m_strData.m_strInfoType = "温馨提示";
    m_strData.m_strContent = QString("%1 登录成功").arg(userInfo.strName);
    m_strData.m_color = gc_colorGreen;
    OperateRecordInfo::AddRecordData(m_strData);
}

void ShearDrivenMaster::on_MainWindow_customContextMenuRequested_unit()
{
    QMenu* pMenu = new QMenu(this);
    QMenu* pMenuLanguage = pMenu->addMenu("语言");
    QAction* language1 = new QAction("中文", this);
    QAction* language2 = new QAction("英文", this);
    QAction* language3 = new QAction("法文", this);
    QAction* version = new QAction("版本", this);
    QAction* quitCurUser = new QAction(QString("%1 退出登录").arg(m_UserINfo.strName), this);
    QAction* quit = new QAction("退出", this);
    if (m_strCurLanguage == gc_English)
    {
        pMenuLanguage->setTitle("Language");
        language1->setText("Chinese");
        language2->setText("English");
        language3->setText("French");
        version->setText("Version");
        quitCurUser->setText(QString("%1 QuitLogin").arg(m_UserINfo.strName));
        quit->setText("Exit");
    }
    else if (m_strCurLanguage == gc_French)
    {
        pMenuLanguage->setTitle("Langue");
        language1->setText("Chinois");
        language2->setText("Anglais");
        language3->setText("Français");
        version->setText("Versions");
        quitCurUser->setText(QString("%1 Se déconnecter du login").arg(m_UserINfo.strName));
        quit->setText("Quitter");
    }
    pMenuLanguage->addAction(language1);
    pMenuLanguage->addAction(language2);
    pMenuLanguage->addAction(language3);

    //添加菜单项
    pMenu->addAction(version);
    pMenu->addAction(quitCurUser);
    pMenu->addAction(quit);

    QFont font = pMenu->font();
    font.setPointSize(16);
    pMenu->setFont(font);
    font = pMenuLanguage->font();
    font.setPointSize(14);
    pMenuLanguage->setFont(font);

    //绑定槽函数
    connect(language1, SIGNAL(triggered()), this, SLOT(on_Language1_TriggerdSlot()));
    connect(language2, SIGNAL(triggered()), this, SLOT(on_Language2_TriggerdSlot()));
    connect(language3, SIGNAL(triggered()), this, SLOT(on_Language3_TriggerdSlot()));
    connect(version, SIGNAL(triggered()), this, SLOT(on_Version_TriggerdSlot()));
    connect(quitCurUser, SIGNAL(triggered()), this, SLOT(on_quitLogin_TriggerdSlot()));
    connect(quit, SIGNAL(triggered()), this, SLOT(on_quit_TriggerdSlot()));

    /* 在鼠标右键处显示菜单 */
    pMenu->exec(cursor().pos());

    /* 释放内存 */
    QList<QAction*> list = pMenu->actions();
    foreach(QAction * pAction, list) delete pAction;
    delete pMenu;
}

void ShearDrivenMaster::on_MainWindow_customContextMenuRequested(const QPoint& pos)
{
    on_MainWindow_customContextMenuRequested_unit();
}

void ShearDrivenMaster::on_Language1_TriggerdSlot()
{
    m_strCurLanguage = gc_Chinese;
    QString strLang = m_CLanguageMgr->GetCurLanguage();
    if (strLang.compare(gc_Chinese, Qt::CaseInsensitive) == 0)
        return;
    m_CLanguageMgr->SetLastLanguage(strLang);
    m_ChangeType = toChinese;
    ChangeUIControlText(m_ChangeType);
    
    m_strData.m_strInfoType = "温馨提示";
    m_strData.m_strContent = "中文切换成功！";
    m_strData.m_color = gc_colorGreen;
    OperateRecordInfo::AddRecordData(m_strData);
    //最后设置
    m_CLanguageMgr->SetCurLanguage(gc_Chinese);
}

void ShearDrivenMaster::on_Language2_TriggerdSlot()
{
    m_strCurLanguage = gc_English;
    QString strLang = m_CLanguageMgr->GetCurLanguage();
    if (strLang.compare(gc_English, Qt::CaseInsensitive) == 0)
        return;
    m_CLanguageMgr->SetLastLanguage(strLang);
    m_CLanguageMgr->SetCurLanguage(gc_English);
   
    m_ChangeType = InDtoForeign;
    if (strLang.compare(gc_Chinese, Qt::CaseInsensitive) == 0)
        m_ChangeType = DtoForeign;
    ChangeUIControlText(m_ChangeType);
   
    m_strData.m_strInfoType = "温馨提示";
    m_strData.m_strContent = "英文切换成功！";
    m_strData.m_color = gc_colorGreen;
    OperateRecordInfo::AddRecordData(m_strData);
}

void ShearDrivenMaster::on_Language3_TriggerdSlot()
{
    m_strCurLanguage = gc_French;
    QString strLang = m_CLanguageMgr->GetCurLanguage();
    if (strLang.compare(gc_French, Qt::CaseInsensitive) == 0)
        return;
    m_CLanguageMgr->SetLastLanguage(strLang);
    m_CLanguageMgr->SetCurLanguage(gc_French);

    m_ChangeType = InDtoForeign;
    if (strLang.compare(gc_Chinese, Qt::CaseInsensitive) == 0)
        m_ChangeType = DtoForeign;
    ChangeUIControlText(m_ChangeType);
   
    m_strData.m_strInfoType = "温馨提示";
    m_strData.m_strContent = "法文切换成功！";
    m_strData.m_color = gc_colorGreen;
    OperateRecordInfo::AddRecordData(m_strData);
}

void ShearDrivenMaster::on_Version_TriggerdSlot()
{
    ErrorCodeTranslate::Instance()->ShowErrinfoDetail("G_1045", InfoType::GeneralType);
    return;
    // 测试模态弹窗
    _tagPromptData promptData;
    promptData.m_bModel = true;
    promptData.m_strTitle = "温馨提示";
    promptData.m_strContent = "当前版本：V1.0.0.0";
    promptData.m_iBtnType = 1; // 无按钮
    OperatePromptInfo::AddPromptInfo(promptData);
}

bool ShearDrivenMaster::CloseMainwindowUnit()
{
    InfoPromptWidget infoWidget;
    BUTTON_STATE ret = infoWidget.Show("关闭确认", "您确定要关闭软件吗？", true, 3, Qt::red, gc_colorLightGreen);

    if (BUTTON_STATE::SURE != ret)
    {
        return false;
    }

    StopSchedulerTask();
    PostRobotEvent("MotorOff");
    QThread::msleep(100);

    RecordData data;
    data.m_strInfoType = "温馨提示";
    data.m_strContent = "程序人为退出！";
    data.m_color = gc_colorBlack;
    OperateRecordInfo::AddRecordData(data);

    m_bAlreadyCloseWindow = true;
    return true;
}

void ShearDrivenMaster::on_quitLogin_TriggerdSlot()
{
    m_strData.m_strInfoType = "温馨提示";
    m_strData.m_strContent = QString("%1 退出登录").arg(m_UserINfo.strName);
    m_strData.m_color = gc_colorBlack;
    emit SignalBacktoLoginDlg();
}

void ShearDrivenMaster::on_quit_TriggerdSlot()
{
    if(CloseMainwindowUnit())
        this->close();
}

void ShearDrivenMaster::closeEvent(QCloseEvent*e)
{
    if (m_bAlreadyCloseWindow)
        return;
    if (CloseMainwindowUnit())
        this->close();
    else
        e->ignore();
}

void ShearDrivenMaster::on_ChipSeting_Clicked()
{
    if (!m_pLocalDataWidget.isNull())
        m_pLocalDataWidget.data()->show();
}

void ShearDrivenMaster::On_Debug_Clicked()
{
    //TestOwnerCode();
    if (!m_Debugdlg.isNull())
        m_Debugdlg.data()->show();
}

void ShearDrivenMaster::On_ImagerSeting_Clicked()
{
    m_pImagerMgrWidget->show();
}

// 涂布机设置
void ShearDrivenMaster::On_CoaterSetting_Clicked()
{
    m_pCoaterMgrWidget->show();
}

// 系统初始化
void ShearDrivenMaster::On_SystemInit_Clicked()
{
    m_strData.m_strInfoType = "界面操作";
    m_strData.m_strContent = "机械臂初始化";
    m_strData.m_color = gc_colorBlue;
    OperateRecordInfo::AddRecordData(m_strData);
    PostRobotEvent("MotorInit");

}

// 报警重置
void ShearDrivenMaster::On_WarningReset_Clicked()
{
    m_strData.m_strInfoType = "界面操作";
    m_strData.m_strContent = "报警重置";
    m_strData.m_color = gc_colorBlue;
    OperateRecordInfo::AddRecordData(m_strData);

    PostRobotEvent("MotorReset");

    //涂布机报警重置
    BgiMpsEvent* p_event2 = new BgiMpsEvent("Driver_Event_Coater");
    p_event2->SetAction("AlarmReset");
    BgiMasterApp* app2 = BgiMasterApp::Instance();
    QApplication::postEvent(app2, p_event2);

    //光机报警重置
    BgiMpsEvent* pEvent3 = new BgiMpsEvent("Driver_Event_Imager");
    pEvent3->SetAction("Reset");
    BgiMasterApp* app3 = BgiMasterApp::Instance();
    QApplication::postEvent(app3, pEvent3);
}


// 一键拍照
void ShearDrivenMaster::On_OneKeyPhoto_Clieked()
{

}

// 错误信息
void ShearDrivenMaster::On_ErrorInfo_Clicked()
{
    if(!m_pErrorLogWidget.isNull())
        m_pErrorLogWidget.data()->show();
}

// 接受弹窗信息
void ShearDrivenMaster::SlotOpePromptInfo(_tagPromptData data)
{
    // 区分模态或者非模态
    if (data.m_bModel)
    {
        if (m_pInfoPrompt->isHidden())
        {
            m_pInfoPrompt->Show(data.m_strTitle, data.m_strContent, data.m_bModel, data.m_iBtnType, data.m_colForeground, data.m_colBackground);
        }
        else
        {
            m_pInfoPrompt->SetTitle(data.m_strTitle, data.m_colForeground, data.m_colBackground);
            m_pInfoPrompt->DisplayButton(data.m_iBtnType);
            m_pInfoPrompt->SetPromptInfo(data.m_strContent);
        }

    }
    else
    {
        // 处理弹窗信息
        InfoPromptWidget* pWidget = new InfoPromptWidget();
        pWidget->Show(data.m_strTitle, data.m_strContent, data.m_bModel, data.m_iBtnType, data.m_colForeground, data.m_colBackground);
        pWidget->setAttribute(Qt::WA_DeleteOnClose);  // 关闭窗口自动析构
    }
}

void ShearDrivenMaster::SetRobotWidgetCtrl(QGroupBox* qRobotGroupBox, QGroupBox* qRobotParaGroupBox)
{
    m_qRobotGroupBox = qRobotGroupBox;
    m_qRobotParaGroupBox = qRobotParaGroupBox;
}

void ShearDrivenMaster::InitWidget()
{
    m_pLayoutMainTitle = this->findChild<QHBoxLayout*>("horizontalLayout_mainTitle");
    QLayout* layout = dynamic_cast<QLayout*>(m_pLayoutMainTitle);
    m_spMainTitleWidget = QSharedPointer<MainTitleWidget>(new MainTitleWidget(layout));
    connect(m_spMainTitleWidget.data(), &MainTitleWidget::SignalClickAbout, this, [this] {on_MainWindow_customContextMenuRequested_unit(); });
    connect(this, &ShearDrivenMaster::SignalChangeAboutButtonText, m_spMainTitleWidget.data(), &MainTitleWidget::SlotChangeAboutButtonText);

    m_pImagerWidget = QSharedPointer<ImagerWidget>(new ImagerWidget());
    ui.LayoutImager->addWidget(m_pImagerWidget.data());

    m_pRobotWidget = QSharedPointer<RobotWidget>(new RobotWidget(this));
    ui.LayoutRobot->addWidget(m_pRobotWidget.data());

    m_pCoaterWidget = QSharedPointer<CoaterWidget>(new CoaterWidget());
    ui.LayoutCoater->addWidget(m_pCoaterWidget.data());

    m_pSequencingWidget = QSharedPointer<SequencingWidget>(new SequencingWidget());
    ui.LayoutSequencing->addWidget(m_pSequencingWidget.data());

    m_pMessageWidget = QSharedPointer<MessageWidget>(new MessageWidget());
    ui.LayoutMessage->addWidget(m_pMessageWidget.data());
    //一起显示，解决启动界面看上去闪烁的现象
    /*m_pImagerWidget.data()->show();
    m_pRobotWidget.data()->show();
    m_pCoaterWidget.data()->show();
    m_pSequencingWidget.data()->show();
    m_pMessageWidget.data()->show();*/

    if (m_qRobotGroupBox == nullptr || m_qRobotParaGroupBox == nullptr)
    {
        m_strData.m_strInfoType = "界面操作";
        m_strData.m_strContent = "GroupBox对象为空,程序无法正常使用";
        m_strData.m_color = gc_colorRed;
        OperateRecordInfo::AddRecordData(m_strData);
    }
    m_Debugdlg = QSharedPointer<EpsonRobotDebug>(new EpsonRobotDebug(this, m_qRobotGroupBox,m_qRobotParaGroupBox));
    ui.ButtonRobotSeting->setEnabled(false);

    m_pRobotConnectParaControl = new RobotConnectParaControl();
    m_pRobotConnectParaControl->SetParentWidget(ui.ButtonRobotSeting, m_pRobotWidget.data(), m_Debugdlg.data());

    //本地数据
    m_pLocalDataWidget = QSharedPointer<LocalDataWidgeet>(new LocalDataWidgeet(this));
    //本地数据与测序界面绑定
    connect(m_pLocalDataWidget.data(), &LocalDataWidgeet::SignalSaveLocalcfg, m_pSequencingWidget.data(), &SequencingWidget::SlotSaveLocalcfg);
    //本地数据与机械臂调试界面绑定
    connect(m_pLocalDataWidget.data(), &LocalDataWidgeet::SignalUpdatePosCombox, m_Debugdlg.data(), &EpsonRobotDebug::SlotUpdatePosCombox);

    m_pImagerMgrWidget = QSharedPointer<ImagerMgrWidget>(new ImagerMgrWidget(this));
    connect(m_pImagerWidget.data(), &ImagerWidget::SignalBasecallSetting, m_pImagerMgrWidget.data(), &ImagerMgrWidget::ClickBasecallSetting);

    m_pCoaterMgrWidget = QSharedPointer<CoaterMgrWidget>(new CoaterMgrWidget(this));

    m_pSchedulerDriver = MasterDevices::Instance()->GetSchedulerDriver();
    //调度任务结束通知
    connect(m_pSchedulerDriver, SIGNAL(SignalSchedulerTaskEnd()), this, SLOT(SetStopRunState()));
    //调度更新Cycle通知
    connect(m_pSchedulerDriver, SIGNAL(SignalUpdateCycle(GeneralParaType::SequencingTableDesc)), m_pSequencingWidget.data(), SLOT(SlotSaveLocalcfg(GeneralParaType::SequencingTableDesc)));
    connect(m_pSchedulerDriver, SIGNAL(SignalUpdateCycle(GeneralParaType::SequencingTableDesc)), m_pLocalDataWidget.data(), SLOT(SlotUpdateCycle(GeneralParaType::SequencingTableDesc)));
    //调度新任务开始通知
    connect(m_pSchedulerDriver, SIGNAL(SignalImagerTaskStart()), m_pImagerWidget.data(), SLOT(InitRunStatus()));
    
    m_pErrorLogWidget = QSharedPointer<ErrorLogWidget>(new ErrorLogWidget(this));

    connect(ui.ButtonChipSeting, SIGNAL(clicked()), this, SLOT(on_ChipSeting_Clicked()));
    connect(ui.ButtonImagerSeting, SIGNAL(clicked()), this, SLOT(On_ImagerSeting_Clicked()));
    connect(ui.ButtonRobotSeting, SIGNAL(clicked()), this, SLOT(On_Debug_Clicked()));
    connect(ui.ButtonCoaterSeting, SIGNAL(clicked()), this, SLOT(On_CoaterSetting_Clicked()));
    connect(ui.ButtonSystemInit, SIGNAL(clicked()), this, SLOT(On_SystemInit_Clicked()));
    connect(ui.ButtonAlarmReset, SIGNAL(clicked()), this, SLOT(On_WarningReset_Clicked()));
    connect(ui.ButtonTakePhotos, SIGNAL(clicked()), this, SLOT(On_OneKeyPhoto_Clieked()));
    connect(ui.ButtonQueryLogs, SIGNAL(clicked()), this, SLOT(On_ErrorInfo_Clicked()));

    connect(ui.ButtonStartRun, SIGNAL(clicked()), this, SLOT(On_StartRun_Clicked()));
    connect(ui.ButtonPauseRun, SIGNAL(clicked()), this, SLOT(On_PauseRun_Clieked()));
    connect(ui.ButtonStopRun, SIGNAL(clicked()), this, SLOT(On_StopRun_Clicked()));
    connect(ui.ButtonSetCycle, SIGNAL(clicked()), this, SLOT(On_SetCycle_Clicked()));
}

// 设置样式
void ShearDrivenMaster::SetStyleSheet()
{
    // 功能选择区样式-20
    // 设置样式
    OpeStyleSheet styleSheet;
    QString strStyle;
    bool bRet = styleSheet.ReadQPushbuttonStyleSheet(strStyle, 20);
    if (bRet)
    {
        ui.ButtonChipSeting->setStyleSheet(strStyle);
        ui.ButtonImagerSeting->setStyleSheet(strStyle);
        ui.ButtonRobotSeting->setStyleSheet(strStyle);
        ui.ButtonCoaterSeting->setStyleSheet(strStyle);
        ui.ButtonSystemInit->setStyleSheet(strStyle);
        ui.ButtonAlarmReset->setStyleSheet(strStyle);
        ui.ButtonTakePhotos->setStyleSheet(strStyle);
        ui.ButtonQueryLogs->setStyleSheet(strStyle);
    }
}

//void ShearDrivenMaster::CreateToolBar()
//{
//    m_mainToolBar = new QToolBar(this);
//    QWidget* spacer = new QWidget(this);
//    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    m_mainToolBar->addWidget(spacer);
//    //m_mainToolBar->setAllowedAreas(Qt::RightToolBarArea|Qt::BottomToolBarArea);
//    m_mainToolBar->setMovable(false);
//    addToolBar(Qt::BottomToolBarArea, m_mainToolBar);
//    QAction* about = new QAction("关于", this);
//    about->setObjectName("obj_about");
//    m_mainToolBar->addSeparator();
//    QFont* font = new QFont();
//    font->setPixelSize(16);
//    m_mainToolBar->setFont(*font);
//    m_mainToolBar->addAction(about);
//    m_mainToolBar->addSeparator();
//    connect(about, &QAction::triggered, this, [this] {QPoint pos(0, 0); on_MainWindow_customContextMenuRequested(pos); });
//   
//
//}

void ShearDrivenMaster::ChangeUIControlText(ChangeType type)
{
    m_CLanguageMgr->ChangeUIControlTextUnit(type,ui.ButtonStartRun);
    m_CLanguageMgr->ChangeUIControlTextUnit(type,ui.ButtonPauseRun);
    m_CLanguageMgr->ChangeUIControlTextUnit(type,ui.ButtonStopRun);
    m_CLanguageMgr->ChangeUIControlTextUnit(type,ui.ButtonSetCycle);
    m_CLanguageMgr->ChangeUIControlTextUnit(type,ui.ButtonChipSeting);
    m_CLanguageMgr->ChangeUIControlTextUnit(type,ui.ButtonImagerSeting);
    m_CLanguageMgr->ChangeUIControlTextUnit(type,ui.ButtonRobotSeting);
    m_CLanguageMgr->ChangeUIControlTextUnit(type,ui.ButtonCoaterSeting);
    m_CLanguageMgr->ChangeUIControlTextUnit(type,ui.ButtonSystemInit);
    m_CLanguageMgr->ChangeUIControlTextUnit(type,ui.ButtonAlarmReset);
    m_CLanguageMgr->ChangeUIControlTextUnit(type,ui.ButtonTakePhotos);
    m_CLanguageMgr->ChangeUIControlTextUnit(type,ui.ButtonQueryLogs);
    m_CLanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label);
    m_CLanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.LabelImager);
    m_CLanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.LabelRobot);
    m_CLanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.LabelCoater);
    m_CLanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.LabelSequencing);
    m_CLanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.LabelMessage);
    if (m_strCurLanguage == gc_Chinese)
        emit SignalChangeAboutButtonText("关于");
    else if (m_strCurLanguage == gc_English)
        emit SignalChangeAboutButtonText("About");
    else if (m_strCurLanguage == gc_French)
        emit SignalChangeAboutButtonText("À propos");
    
    m_pImagerWidget.data()->ChangeUIControlText(m_ChangeType, m_strCurLanguage);
    m_pRobotWidget.data()->ChangeUIControlText(m_ChangeType, m_strCurLanguage);
    m_pCoaterWidget.data()->ChangeUIControlText(m_ChangeType, m_strCurLanguage);
    m_Debugdlg.data()->ChangeUIControlText(m_ChangeType, m_strCurLanguage);
}

void ShearDrivenMaster::PostRobotEvent(QString strActionName)
{
    BgiMpsEvent* p_event = new BgiMpsEvent("Driver_Event_Epson_Robot");
    p_event->SetAction(strActionName);
    BgiMasterApp* app = BgiMasterApp::Instance();
    QApplication::postEvent(app, p_event);
}

void ShearDrivenMaster::TestOwnerCode()
{
    /*m_pInfoPrompt->SetTitle(StringConversionEncoding("参数设置"), true);
    m_pInfoPrompt->SetPromptInfo(StringConversionEncoding("注液表轮次设置有误，请重新检查设置后点击准备界面 “确定” 按钮！"));
    if (m_bShowTwobtn)
    {
        m_pInfoPrompt->DisplayButton(3);
        m_bShowTwobtn = false;
    }
    else
    {
        m_pInfoPrompt->DisplayButton(1);
        m_bShowTwobtn = true;
    }
    m_pInfoPrompt->DisplayDialog(true);
    */

    // 测试模态弹窗
    _tagPromptData promptData;
    promptData.m_bModel = true;
    promptData.m_strTitle = "测试模态弹窗";
    promptData.m_strContent = "测试1";
    promptData.m_iBtnType = 4; // 无按钮
    OperatePromptInfo::AddPromptInfo(promptData);


    /*m_strData.m_strInfoType = "温馨提示";
    m_strData.m_strContent = "打开机械臂设置！";
    m_strData.m_color = gc_colorBlack;
    m_strData.m_bWrite = true;
    OperateRecordInfo::AddRecordData(m_strData);*/

}

void ShearDrivenMaster::On_StartRun_Clicked()
{
    QString curBarcode = m_pSequencingWidget->GetCurBarcode();

    InfoPromptWidget infoWidget;
    BUTTON_STATE ret = BUTTON_STATE::NO_BUTTON;
    if (ui.ButtonStopRun->isEnabled())
    {
        //暂停后恢复运行
        ret = infoWidget.Show("恢复确认", "确定要恢复测序吗？", true, 3, Qt::red, gc_colorLightBlue);
    }
    else
    {
        //第一次开始运行，显示详细信息供用户确认
        GeneralParaType::SqlCfgData chipData;
        if (!OperateSqliteConfig::Instance()->QureyCfgDataByBarcode(curBarcode, chipData))
        {
            QMessageBox::critical(this, "错误", "查询不到芯片信息，无法开始任务！");
            return;
        }
        QString info = "Barcode=" + chipData.strBarCode
            + "\r\n生化Cycle=" + QString::number(chipData.strTableDesc.iBiochemistryTimes) + "，拍照Cycle=" + QString::number(chipData.strTableDesc.iPhotographTime)
            + "\r\nR1=" + QString::number(chipData.strTableDesc.iR1) + "，R2=" + QString::number(chipData.strTableDesc.iR2) + "，B1=" + QString::number(chipData.strTableDesc.iB1) + "，B2=" + QString::number(chipData.strTableDesc.iB2);
        
        int endCycle = m_pSchedulerDriver->GetEndCycle();
        info += "\r\n回收Cycle=" + ((endCycle > 0) ? QString::number(endCycle) : "无");

        ImagerParaType::ImagerConfigPara data;
        if (MasterDevices::Instance()->GetImagerDriver()->ImagerConfigGet(data))
        {
            info += "\r\n拍照行=" + QString::number(data.StartRow) + "~" + QString::number(data.EndRow) + "，列=" + QString::number(data.StartCol) + "~" + QString::number(data.EndCol) + "，存图数=" + QString::number(data.SaveFovString.split(';', Qt::SkipEmptyParts).count());
        }

        ret = infoWidget.Show("开始运行确认", info + "\r\n确定要开始测序吗？", true, 3, Qt::red, gc_colorLightBlue);
    }
    if (BUTTON_STATE::SURE != ret)
    {
        return;
    }

    BGI_MESSAGE("StartRun");

    m_pLocalDataWidget.data()->SetSaveBtnState(false);
    ui.ButtonStartRun->setEnabled(false);
    ui.ButtonPauseRun->setEnabled(true);
    ui.ButtonStopRun->setEnabled(true);
    m_Debugdlg.data()->hide();
    ui.ButtonRobotSeting->setEnabled(false);
    m_pRobotWidget.data()->SetRunState(true);
    BgiMpsEvent* p_event = new BgiMpsEvent("Driver_Event_Scheduler");
    p_event->SetAction("StartRun");
    
    p_event->AddParams("Param1", curBarcode);
    BgiMasterApp* app = BgiMasterApp::Instance();
    QApplication::postEvent(app, p_event);

    m_strData.m_strInfoType = "界面操作";
    m_strData.m_strContent = "开始运行，芯片号是：" + curBarcode;
    m_strData.m_color = gc_colorBlue;
    OperateRecordInfo::AddRecordData(m_strData);
}

void ShearDrivenMaster::On_PauseRun_Clieked()
{
    InfoPromptWidget infoWidget;
    BUTTON_STATE ret = infoWidget.Show("暂停确认", "确定要暂停测序吗？", true, 3, Qt::red, gc_colorLightBlue);
    if (BUTTON_STATE::SURE != ret)
    {
        return;
    }

    BGI_MESSAGE("PauseRun");

    ui.ButtonStartRun->setEnabled(true);
    ui.ButtonPauseRun->setEnabled(false);
    ui.ButtonStopRun->setEnabled(true);

    BgiMpsEvent* p_event = new BgiMpsEvent("Driver_Event_Scheduler");
    p_event->SetAction("PauseRun");
    BgiMasterApp* app = BgiMasterApp::Instance();
    QApplication::postEvent(app, p_event);

    m_strData.m_strInfoType = "界面操作";
    m_strData.m_strContent = "暂停运行";
    m_strData.m_color = gc_colorBlue;
    OperateRecordInfo::AddRecordData(m_strData);
}

void ShearDrivenMaster::On_StopRun_Clicked()
{
    InfoPromptWidget infoWidget;
    BUTTON_STATE ret = infoWidget.Show("急停确认", "确定要紧急停止测序吗？", true, 3, Qt::red, gc_colorLightBlue);
    if (BUTTON_STATE::SURE != ret)
    {
        return;
    }

    BGI_MESSAGE("StopRun");

    //SetStopRunState();
    StopSchedulerTask();

    m_strData.m_strInfoType = "界面操作";
    m_strData.m_strContent = "紧急停止";
    m_strData.m_color = gc_colorBlue;
    OperateRecordInfo::AddRecordData(m_strData);
}

void ShearDrivenMaster::On_SetCycle_Clicked()
{
    bool bRet = false;
    int cycle = ui.LineSycleNum->text().toInt(&bRet);
    if (!bRet)
    {
        QMessageBox::critical(this, "错误", "请设置正确的回收Cycle值！");
        return;
    }
    BGIIputDialog dlg;
    cycle = dlg.GetInt("设置回收Cycle", "回收Cycle值：", &bRet,cycle, 0, 200);
    if (!bRet)
        return;
    ui.LineSycleNum->setText(QString("%1").arg(cycle));
    QString str = "SetEndCycle " + QString::number(cycle);
    BGI_MESSAGE(str.toStdString().c_str());

    BgiMpsEvent* p_event = new BgiMpsEvent("Driver_Event_Scheduler");
    p_event->SetAction("SetEndCycle");
    p_event->AddParams("Param1", cycle);
    BgiMasterApp* app = BgiMasterApp::Instance();
    QApplication::postEvent(app, p_event);
}

void ShearDrivenMaster::StopSchedulerTask()
{
    BgiMpsEvent* p_event = new BgiMpsEvent("Driver_Event_Scheduler");
    p_event->SetAction("StopRun");
    BgiMasterApp* app = BgiMasterApp::Instance();
    QApplication::postEvent(app, p_event);
}

void ShearDrivenMaster::SetStopRunState()
{
    ui.ButtonStartRun->setEnabled(true);
    ui.ButtonPauseRun->setEnabled(false);
    ui.ButtonStopRun->setEnabled(false);
    m_pLocalDataWidget.data()->SetSaveBtnState(true);
    ui.ButtonRobotSeting->setEnabled(true);
    m_pRobotWidget.data()->SetRunState(false);
}

