#include "ModifyUserInfo.h"
#include <QIcon>
#include "OperateSqliteConfig.h"
#include "InfoPromptWidget.h"


ModifyUserInfo::ModifyUserInfo(QWidget *parent)
	: QDialog(parent)
	, m_iOperaType(0)
{
	ui.setupUi(this);
	this->setWindowIcon(QIcon(":/ShearDrivenMaster/2.ico"));

	setWindowModality(Qt::ApplicationModal);
	Qt::WindowFlags windowFlag = windowFlags() & ~Qt::WindowMinimizeButtonHint &~Qt::WindowMaximizeButtonHint;  // 始终显示在最上层
	windowFlag |= Qt::WindowStaysOnTopHint;
	setWindowFlags(windowFlag);

	//添加背景图
	QImage image1;
	image1.load(":/ShearDrivenMaster/Login.jpg");
	QImage image2 = image1.scaled(200, 150);
	QPalette palette;
	palette.setBrush(this->backgroundRole(), QBrush(image2));//设置背景
	setPalette(palette);

	m_pInfoPrompt = new InfoPromptWidget((QDialog*)this);

	connect(ui.pushButton_SureInfo, SIGNAL(clicked()), this, SLOT(On_ModifyOk_Clicked()));
	connect(ui.pushButton_CancelInfo, SIGNAL(clicked()), this, SLOT(On_ModifyCancel_Clicked()));
}

ModifyUserInfo::~ModifyUserInfo()
{}

void ModifyUserInfo::SetUserInfo(const GeneralParaType::UserInfo userInfo, int iOperaType)
{
	m_userInfo = userInfo;
	m_iOperaType = iOperaType;
}

void ModifyUserInfo::TipInfo(QString strContent, bool bColorRed)
{
	m_pInfoPrompt->SetTitle("温馨提示", bColorRed);
	m_pInfoPrompt->SetPromptInfo(strContent);
	m_pInfoPrompt->DisplayButton(1);
	m_pInfoPrompt->DisplayDialog(true);
}

void ModifyUserInfo::On_ModifyOk_Clicked()
{
	QString strOldPassword = ui.lineEdit_OldPassword->text().trimmed();
	if (strOldPassword.isEmpty())
	{
		TipInfo("旧密码不能为空!");
		return;
	}

	GeneralParaType::UserInfo userINfo;
	QString strErrorInfo = "";
	if(!OperateSqliteConfig::Instance()->QueryUserInfoByName(m_userInfo.strName, userINfo,strErrorInfo))
		TipInfo(strErrorInfo);
	if (userINfo.password != strOldPassword)
	{
		TipInfo("输入的旧密码不正确，请重新输入！");
		return;
	}
	if (1 == m_iOperaType)
	{
		if (!OperateSqliteConfig::Instance()->DelUser(userINfo.strName))
		{
			TipInfo("用户删除失败");
		}
		TipInfo("用户删除成功", false);
		this->close();
	}
	else if (2 == m_iOperaType)
	{
		if (!OperateSqliteConfig::Instance()->UpdateUser(m_userInfo))
		{
			TipInfo("用户信息更新失败");
		}
		TipInfo("用户信息更新成功", false);
		this->close();
	}
	else
		TipInfo("未知操作!");
}

void ModifyUserInfo::On_ModifyCancel_Clicked()
{
	ui.lineEdit_OldPassword->setText("");
}