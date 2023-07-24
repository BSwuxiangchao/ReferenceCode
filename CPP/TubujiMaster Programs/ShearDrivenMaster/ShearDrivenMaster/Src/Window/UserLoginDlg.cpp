#include "UserLoginDlg.h"
#include <QMenu>
#include "ShearDrivenMaster.h"
#include "BgiMasterApp.h"
#include "ParaType.h"
#include "ModifyUserInfo.h"
#include "OperateSqliteConfig.h"
#include "InfoPromptWidget.h"
#include "OperateSqliteConfig.h"
#include "BGIIputDialog.h"

UserLoginDlg::UserLoginDlg(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	this->setWindowIcon(QIcon(":/ShearDrivenMaster/2.ico"));
	setWindowModality(Qt::ApplicationModal);
	Qt::WindowFlags windowFlag = windowFlags() & ~Qt::WindowMaximizeButtonHint&Qt::WindowCloseButtonHint;
	setWindowFlags(windowFlag);
	setFixedSize(this->width(),this->height());
	
	InitWidget();
	connect(ui.pushButton_Login, SIGNAL(clicked()), this, SLOT(On_Login_Clicked()));
	connect(ui.pushButton_Update, SIGNAL(clicked()), this, SLOT(On_Update_Clicked()));
	connect(ui.pushButton_Cancle, SIGNAL(clicked()), this, SLOT(On_Cancel_Clicked()));

	//创建用户信息表
	OperateSqliteConfig::Instance()->CreateUserTable();

	ui.lineEdit_UserName->setText("BGI");
	ui.lineEdit_Password->setText("5");
	//TestCode();

}

UserLoginDlg::~UserLoginDlg()
{}

void UserLoginDlg::InitWidget()
{
	on_func1_TriggerdSlot();
	//添加背景图
	QImage image1;
	image1.load(":/ShearDrivenMaster/Login.jpg");
	QImage image2 = image1.scaled(490, 300);
	QPalette palette;
	palette.setBrush(this->backgroundRole(), QBrush(image2));//设置背景
	setPalette(palette);
	//创建工具栏
	m_LoginToolBar = new QToolBar(this);
	QWidget* spacer = new QWidget(this);
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_LoginToolBar->addWidget(spacer);
	addToolBar(Qt::TopToolBarArea, m_LoginToolBar);
	QAction* about = new QAction("帮助", this);
	about->setObjectName("obj_about");
	m_LoginToolBar->addSeparator();
	QFont* font = new QFont();
	font->setPixelSize(16);
	m_LoginToolBar->setFont(*font);
	m_LoginToolBar->addAction(about);
	m_LoginToolBar->addSeparator();
	m_LoginToolBar->setMovable(false);
	connect(about, &QAction::triggered, this, [this] {SlotCreateMenu(); });

	m_pModifyUserInfo = QSharedPointer<ModifyUserInfo>(new ModifyUserInfo());
	m_pInfoPrompt = new InfoPromptWidget((QDialog*)this);

}

void UserLoginDlg::TipInfo(QString strContent, bool bColorRed)
{
	m_pInfoPrompt->SetTitle(StringConversionEncoding("温馨提示"), bColorRed);
	m_pInfoPrompt->SetPromptInfo(strContent);
	m_pInfoPrompt->DisplayButton(1);
	m_pInfoPrompt->DisplayDialog(true);
}

void UserLoginDlg::On_Login_Clicked()
{
	QString strName = ui.lineEdit_UserName->text().trimmed();
	/*if (strName.isEmpty())
	{
		TipInfo("用户名不能为空！");
		return;
	}*/
	if (ui.pushButton_Login->text() == "登录")
	{
		
		GeneralParaType::UserInfo userINof;
		userINof.userType = GeneralParaType::UserType::Admin;
		
		QString strPassword = ui.lineEdit_Password->text().trimmed();
		/*if (strPassword.isEmpty())
		{
			TipInfo("密码不能为空！");
			return;
		}
		QString strErrorInfo = "";
		if (!OperateSqliteConfig::Instance()->QueryUserInfoByName(strName,userINof, strErrorInfo))
		{
			TipInfo(strErrorInfo);
			return;
		}
		if (strPassword != userINof.password)
		{
			TipInfo("密码不正确，请重新输入！");
			return;
		}*/
		if (m_pShearDrivenMaster.isNull())
		{
			m_pShearDrivenMaster = QSharedPointer<ShearDrivenMaster>(new ShearDrivenMaster());
			////设置状态栏图标
			m_pShearDrivenMaster.data()->setWindowIcon(QIcon(":/ShearDrivenMaster/2.ico"));
			m_pShearDrivenMaster.data()->setWindowState(Qt::WindowMaximized);
			connect(m_pShearDrivenMaster.data(), &ShearDrivenMaster::SignalBacktoLoginDlg, this, &UserLoginDlg::SlotBacktoLoginDlg);
			//启动主线程
			BgiMasterApp* app = BgiMasterApp::Instance();
		}
		m_pShearDrivenMaster.data()->show();
		m_pShearDrivenMaster.data()->SetUserInfo(userINof);
		this->hide();
	}
	else
	{
		GeneralParaType::UserInfo userINof;
		userINof.strName = strName;
		m_pModifyUserInfo.data()->SetUserInfo(userINof,1);
		m_pModifyUserInfo.data()->show();
	}
}

void UserLoginDlg::On_Update_Clicked()
{
	QString strName = ui.lineEdit_UserName->text().trimmed();
	if (strName.isEmpty())
	{
		TipInfo("用户名不能为空！");
		return;
	}
	QString strPassword = ui.lineEdit_Password->text().trimmed();
	if (strPassword.isEmpty())
	{
		TipInfo("密码不能为空！");
		return;
	}
	GeneralParaType::UserInfo userINfo;
	int iIndex = ui.comboBox_UserType->currentIndex();
	if (iIndex == 0)
		userINfo.userType = GeneralParaType::UserType::Operator;
	else if (iIndex == 1)
		userINfo.userType = GeneralParaType::UserType::Maintainer;
	else if (iIndex == 2)
		userINfo.userType = GeneralParaType::UserType::Admin;

	userINfo.strName = strName;
	userINfo.password = strPassword;
	m_pModifyUserInfo.data()->SetUserInfo(userINfo, 2);
	m_pModifyUserInfo.data()->show();
}

void UserLoginDlg::On_Cancel_Clicked()
{
	if (ui.pushButton_Cancle->text() == "取消")
	{
		ui.lineEdit_UserName->setText("");
		ui.lineEdit_Password->setText("");
	}
	else
	{
		QString strName = ui.lineEdit_UserName->text().trimmed();
		if (strName.isEmpty())
		{
			TipInfo("用户名不能为空！");
			return;
		}
		QString strPassword = ui.lineEdit_Password->text().trimmed();
		if (strPassword.isEmpty())
		{
			TipInfo("密码不能为空！");
			return;
		}
		if (OperateSqliteConfig::Instance()->IsUserExit(strName))
		{
			TipInfo("用户已存在！");
			return;
		}
		GeneralParaType::UserInfo userINfo;
		userINfo.strName = strName;
		userINfo.password = strPassword;
		int iIndex = ui.comboBox_UserType->currentIndex();
		if (iIndex == 0)
			userINfo.userType = GeneralParaType::UserType::Operator;
		else if (iIndex == 1)
			userINfo.userType = GeneralParaType::UserType::Maintainer;
		else if (iIndex == 2)
			userINfo.userType = GeneralParaType::UserType::Admin;
		if (OperateSqliteConfig::Instance()->AddUser(userINfo))
			TipInfo("用户注册成功! ", false);
		else
			TipInfo("用户注册失败！");
	}
}

void UserLoginDlg::SlotBacktoLoginDlg()
{
	m_pShearDrivenMaster.data()->hide();
	this->show();
}

void UserLoginDlg::SlotCreateMenu()
{
	QMenu* pMenu = new QMenu(this);
	QAction* func1 = new QAction("登录", this);
	QAction* func2 = new QAction("管理", this);
	pMenu->addAction(func1);
	pMenu->addAction(func2);

	QFont font = pMenu->font();
	font.setPointSize(12);
	pMenu->setFont(font);

	
	//绑定槽函数
	connect(func1, SIGNAL(triggered()), this, SLOT(on_func1_TriggerdSlot()));
	connect(func2, SIGNAL(triggered()), this, SLOT(on_func2_TriggerdSlot()));

	/* 在鼠标右键处显示菜单 */
	pMenu->exec(cursor().pos());
}

void UserLoginDlg::on_func1_TriggerdSlot()
{
	ui.lineEdit_UserName->setText("");
	ui.lineEdit_Password->setText("");
	ui.label_UserType->hide();
	ui.comboBox_UserType->hide();
	ui.comboBox_UserType->setEnabled(false);
	ui.pushButton_Login->setText("登录");
	ui.pushButton_Update->hide();
	ui.pushButton_Update->setEnabled(false);
	ui.pushButton_Cancle->setText("取消");
}

void UserLoginDlg::on_func2_TriggerdSlot()
{
	ui.lineEdit_UserName->setText("");
	ui.lineEdit_Password->setText("");
	ui.label_UserType->show();
	ui.comboBox_UserType->show();
	ui.comboBox_UserType->setEnabled(true);
	ui.pushButton_Login->setText("删除");
	ui.pushButton_Update->show();
	ui.pushButton_Update->setEnabled(true);
	ui.pushButton_Cancle->setText("注册");
}

void UserLoginDlg::TestCode()
{
	for (int i = 0; i < 20; i++)
	{
		_sleep(1000);
		QString test = QString("这是一些错位测试%1").arg(i);
		OperateSqliteConfig::Instance()->RecordErrToDatabase(test, GeneralType);

	}
	QList<GeneralParaType::sqlErrorData> vecSqlErrData;
	OperateSqliteConfig::Instance()->QueryErrInfo(vecSqlErrData, "2023-04-08", "2023-04-11");
	//ErrorCodeTranslate::Instance()->DelErrInfo("2023-04-15");
}