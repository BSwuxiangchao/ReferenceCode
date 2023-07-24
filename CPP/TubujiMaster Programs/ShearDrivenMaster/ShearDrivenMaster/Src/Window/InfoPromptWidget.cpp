#include "InfoPromptWidget.h"
#include "ui_InfoPromptWidget.h"
#include <QCloseEvent>

InfoPromptWidget::InfoPromptWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InfoPromptWidget),
    m_iButtonState(NO_BUTTON)
{
    ui->setupUi(this);
    //Qt::WindowFlags windowFlag = Qt::WindowStaysOnTopHint;  // 始终显示在最上层
    //setWindowFlags(windowFlag);

    //setWindowFlags(Qt::FramelessWindowHint);
    //setWindowFlags(Qt::WindowStaysOnTopHint|Qt::WindowCloseButtonHint);
    // 隐藏对话框帮助和关闭按钮
//    Qt::WindowFlags windowFlag = windowFlags()&~Qt::WindowCloseButtonHint&~Qt::WindowContextHelpButtonHint;
	//Qt::WindowFlags windowFlag = windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowCloseButtonHint;
 //   windowFlag |= Qt::WindowStaysOnTopHint;  // 始终显示在最上层
 //   setWindowFlags(windowFlag);

    // 设置销毁时析构
    //setAttribute(Qt::WA_DeleteOnClose);
}

InfoPromptWidget::~InfoPromptWidget()
{
    this->destroy();
    delete ui; 
}

BUTTON_STATE InfoPromptWidget::Show(bool bModal, const QString& title, bool bColorRed, const QString& info, const int& iButton)
{
    SetTitle(title, bColorRed);
    DisplayButton(iButton);
    SetPromptInfo(info);
    DisplayDialog(bModal);

    return m_iButtonState;
}

BUTTON_STATE InfoPromptWidget::Show(const QString& title, const QString& info, bool bModal, const int& iButton, const QColor& colTitleForeground, const QColor& colTitleBackground)
{
	SetTitle(title, colTitleForeground, colTitleBackground);
	DisplayButton(iButton);
	SetPromptInfo(info);
	DisplayDialog(bModal);

	return m_iButtonState;
}

BUTTON_STATE InfoPromptWidget::Show(const QString& title, const QString& info, const QString& buttonName, const bool& bEnable, bool bModal, const QColor& colTitleForeground, const QColor& colTitleBackground)
{
    SetTitle(title, colTitleForeground, colTitleBackground);
    DisplayButton(buttonName, bEnable);
    SetPromptInfo(info);
    DisplayDialog(bModal);

    return m_iButtonState;
}

// 显示对话框
void InfoPromptWidget::DisplayDialog(bool bModal)
{
    this->setModal(bModal);
	if (bModal)
	{
		this->exec();
	}
	else
	{
		this->show();
	}
}

// 设置标题
void InfoPromptWidget::SetTitle(const QString& title, bool bColorRed)
{
    QString str = "color:blue;background-color:rgb(169,251,193);font:16pt;";
    if(bColorRed)
    {
        str = "color:red;background-color:rgb(169,251,193);font:16pt;";
    }
    ui->labelTitle->setStyleSheet(str);
    ui->labelTitle->setText(title);
}

/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 功能描述：设置窗体标题
 * 时    间：2022/8/10 10:10
 * 入    参：titleText：标题名称；
 * 入    参：colForeground：标题前景色；
 * 入    参：colBackground：标题背景色；
 * 出    参：无
 * 返    回：无
 ************************************* end-XCW *****************************************/
void InfoPromptWidget::SetTitle(const QString& titleText, const QColor& colForeground, const QColor& colBackground)
{
    // 前景色
    int fR = 0;
	int fG = 0;
	int fB = 0;
    // 背景色
	int bR = 0;
	int bG = 0;
	int bB = 0;

    colForeground.getRgb(&fR, &fG, &fB);
    colBackground.getRgb(&bR, &bG, &bB);

    QString strSheet = QString("color:rgb(%1,%2,%3);background-color:rgb(%4,%5,%6);font:16pt;").arg(fR).arg(fG).arg(fB).arg(bR).arg(bG).arg(bB);
    ui->labelTitle->setStyleSheet(strSheet);
    ui->labelTitle->setText(titleText);
}

// 设置提示内容
void InfoPromptWidget::SetPromptInfo(const QString& info)
{
    ui->textEditContext->setText(info);
}
// 设置按钮的显示,1-只显示确定按钮，2-只显示取消，3-显示确定和取消按钮，4-都不显示确定和取消，其他值默认只显示确认按钮
void InfoPromptWidget::DisplayButton(const int& bSure)
{
    //if (bSure == 4)
    //{
    //    Qt::WindowFlags windowFlag = windowFlags() & ~Qt::WindowContextHelpButtonHint & Qt::WindowCloseButtonHint;
    //    windowFlag |= Qt::WindowStaysOnTopHint;  // 始终显示在最上层
    //    setWindowFlags(windowFlag);
    //}
    //else
    //{
    //    Qt::WindowFlags windowFlag = windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowCloseButtonHint;
    //    windowFlag |= Qt::WindowStaysOnTopHint;  // 始终显示在最上层
    //    setWindowFlags(windowFlag);
    //}
    switch (bSure)
    {
    case 1:
    {
		ui->pushButtonSure->show();
		ui->pushButtonCancel->hide();
		// 隐藏弹簧
//		ui->horizontalLayout->removeItem(ui->horizontalSpacer);
        break;
    }
	case 2:
    {
		ui->pushButtonSure->hide();
		ui->pushButtonCancel->show();
		// 隐藏弹簧
//		ui->horizontalLayout->removeItem(ui->horizontalSpacer);
        break;
    }
	case 3:
    {
		ui->pushButtonSure->show();
		ui->pushButtonCancel->show();
		// 显示弹簧
//		ui->horizontalLayout->insertItem(1, ui->horizontalSpacer);
        break;
    }
	case 4:
    {
		ui->pushButtonSure->hide();
		ui->pushButtonCancel->hide();
        m_iButtonState = NO_BUTTON;  // 没有按钮
        break;
    }
    default:
    {
		ui->pushButtonSure->show();
		ui->pushButtonCancel->hide();
		// 隐藏弹簧
//		ui->horizontalLayout->removeItem(ui->horizontalSpacer);
        break;
    }
       
    }
}

void InfoPromptWidget::DisplayButton(const QString& sButtonName, const bool& bEnable)
{
    ui->pushButtonSure->show();
    ui->pushButtonCancel->hide();
    ui->pushButtonSure->setEnabled(bEnable);
    ui->pushButtonSure->setText(sButtonName);
}

// 获取按钮状态
const BUTTON_STATE& InfoPromptWidget::GetButtonState() const
{
    return m_iButtonState;
}

void InfoPromptWidget::closeEvent(QCloseEvent* event)
{
    event->accept();
}

// 确定
void InfoPromptWidget::on_pushButtonSure_clicked()
{
    m_iButtonState = SURE;
    this->close();
}

// 取消
void InfoPromptWidget::on_pushButtonCancel_clicked()
{
    m_iButtonState = CANCEL;
    this->close();
}

