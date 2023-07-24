#include "BGIIputDialog.h"
#include <QCloseEvent>
#include <qspinbox.h>
#include <qlabel.h>
#include <qlineedit.h>

BGIIputDialog::BGIIputDialog(QWidget *parent)
	: QDialog(parent)
    , m_strCurText("")
{
	ui.setupUi(this);
    this->setWindowIcon(QIcon(":/ShearDrivenMaster/2.ico"));
}

BGIIputDialog::~BGIIputDialog()
{
    ui.verticalLayout->deleteLater();
	this->destroy();
}

void BGIIputDialog::SetInputWindowTitle(const QString& title)
{
    this->setWindowTitle(title);
}

void BGIIputDialog::closeEvent(QCloseEvent* event)
{
    event->accept();
}

// 确定
void BGIIputDialog::on_pushButtonSure_clicked()
{
    m_iButtonState = IN_SURE;
    this->close();
}

// 取消
void BGIIputDialog::on_pushButtonCancel_clicked()
{
    m_iButtonState = IN_CANCEL;
    this->close();
}

void BGIIputDialog::ShowDlg(const QString& title, const QString& label, int iType, int value, int minValue, int maxValue)
{
    SetInputWindowTitle(title);
    QFont CtrFont = QFont();
    CtrFont.setPointSize(20);
    QSizePolicy::Policy hor = QSizePolicy::Minimum, ver = QSizePolicy::Preferred;
    QLabel* CtrLabel = new QLabel();
    CtrLabel->setSizePolicy(hor, ver);
    CtrLabel->setFont(CtrFont);
    CtrLabel->setStyleSheet("color:blue;\nbackground-color: rgb(169,251,193); \nfont:16pt;");
    ui.verticalLayout->addWidget(CtrLabel);
    CtrLabel->setText(label);
    if (iType == 0)
    {
        QSpinBox* CtrSpinbox = new QSpinBox();
        CtrSpinbox->setSizePolicy(hor, ver);
        CtrFont.setPointSize(30);
        CtrSpinbox->setFont(CtrFont);
        CtrSpinbox->setAlignment(Qt::AlignmentFlag::AlignHCenter);
        CtrSpinbox->setValue(value);
        CtrSpinbox->setMinimum(minValue);
        CtrSpinbox->setMaximum(maxValue);
        ui.verticalLayout->addWidget(CtrSpinbox);

        this->exec();
        m_strCurText = CtrSpinbox->text();
        delete CtrSpinbox;
    }
    else if (iType == 1)
    {
        QLineEdit* CtrLinedit = new QLineEdit();
        CtrLinedit->setSizePolicy(hor, ver);
        CtrFont.setPointSize(30);
        CtrLinedit->setFont(CtrFont);
        CtrLinedit->setAlignment(Qt::AlignmentFlag::AlignHCenter);
        ui.verticalLayout->addWidget(CtrLinedit);

        this->exec();
        m_strCurText = CtrLinedit->text();
        delete CtrLinedit;
    }
    delete CtrLabel;
}

QString BGIIputDialog::getText(const QString& title, const QString& label, bool* ok)
{
    ShowDlg(title, label, 1);
    if (m_iButtonState == IN_SURE)
        *ok = true;
    else
        *ok = false;
    if(*ok)
        return m_strCurText;
    return "";
}

int BGIIputDialog::GetInt(const QString& title, const QString& label, bool* ok,int value, int minValue,int maxValue)
{
    ShowDlg(title, label, 0,value, minValue, maxValue);
    if (m_iButtonState == IN_SURE)
        *ok = true;
    else
        *ok = false;
    if (!(*ok))
        return -1;
    int iText = m_strCurText.toInt(ok);
    if (!(*ok))
        return -1;
    if (minValue == maxValue)
        return iText;
    if (iText >= minValue && iText <= maxValue)
        return iText;
    *ok = false;
    return -1;
}

double BGIIputDialog::GetDouble(const QString& title, const QString& label, bool* ok,double step,double value, double minValue, double maxValue)
{
    SetInputWindowTitle(title);
    QFont CtrFont = QFont();
    CtrFont.setPointSize(20);
    QSizePolicy::Policy hor = QSizePolicy::Minimum, ver = QSizePolicy::Preferred;
    QLabel* CtrLabel = new QLabel();
    CtrLabel->setSizePolicy(hor, ver);
    CtrLabel->setFont(CtrFont);
    CtrLabel->setStyleSheet("color:blue;\nbackground-color: rgb(169,251,193); \nfont:16pt;");
    ui.verticalLayout->addWidget(CtrLabel);
    CtrLabel->setText(label);
    QDoubleSpinBox* CtrSpinbox = new QDoubleSpinBox();
    CtrSpinbox->setSizePolicy(hor, ver);
    CtrFont.setPointSize(30);
    CtrSpinbox->setFont(CtrFont);
    CtrSpinbox->setAlignment(Qt::AlignmentFlag::AlignHCenter);
    CtrSpinbox->setValue(value);
    CtrSpinbox->setMinimum(minValue);
    CtrSpinbox->setMaximum(maxValue);
    //CtrSpinbox->setStepType()
    ui.verticalLayout->addWidget(CtrSpinbox);

    this->exec();
    m_strCurText = CtrSpinbox->text();
    delete CtrLabel;
    delete CtrSpinbox;

    if (m_iButtonState == IN_SURE)
        *ok = true;
    else
        *ok = false;
    if (!(*ok))
        return -1;
    double iText = m_strCurText.toDouble(ok);
    if (!(*ok))
        return -1;
    if (minValue == maxValue)
        return iText;
    if (iText >= minValue && iText <= maxValue)
        return iText;
    *ok = false;
    return -1;
}