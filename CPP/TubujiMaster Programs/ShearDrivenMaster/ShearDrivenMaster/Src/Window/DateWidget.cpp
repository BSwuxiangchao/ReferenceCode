#include "DateWidget.h"
#include "BgiLogger.h"
#include <qdatetime.h>
#include <qtimer.h>
#include "DeviceDetailCfg.h"
#include "GitVer.h"

DateWidget::DateWidget(QWidget* parent)
    :m_pParentWidget(parent)
{

    m_pVersionLabel = m_pParentWidget->findChild<QLabel*>("label_Version");
    m_pOpenTimeLabel = m_pParentWidget->findChild<QLabel*>("label_OpenSoftware");
    m_pDateLabel = m_pParentWidget->findChild<QLabel*>("label_CurrentTime");

    // 代码上传版本号
    QString strGitVer = GIT_VER;
    QStringList strTemList = strGitVer.split(":");
    if (strTemList.size() >= 4)
        strGitVer = strTemList[3];
    QString strVersion = "";
    DeviceDetailCfg::Instance()->GetCfgValue("GeneralCfg/SoftwareVersion", strVersion);
    m_pVersionLabel->setText(QString("版本:%1 --- Git:%2").arg(strVersion).arg(strGitVer));

    QTimer* timer = new QTimer(this);
    timer->start(1000);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(UpdateTime()));

	// m_pDateLabel 改为显示当前时间
	// m_pTimeLabel 改为表示开机时间：年/月/日 时:分
	QDateTime current_time = QDateTime::currentDateTime();
    QString strCurTime = "开机:";
    QString strTime = current_time.toString("yyyy/MM/dd hh:mm");
    strCurTime += strTime;
    m_pOpenTimeLabel->setText(strCurTime);
    
}

DateWidget::~DateWidget()
{

}

void DateWidget::UpdateTime()
{
    QDateTime current_time = QDateTime::currentDateTime();
    QString str_date = "当前:";
    str_date += current_time.toString("yyyy/MM/dd hh:mm");
    m_pDateLabel->setText(str_date);
}