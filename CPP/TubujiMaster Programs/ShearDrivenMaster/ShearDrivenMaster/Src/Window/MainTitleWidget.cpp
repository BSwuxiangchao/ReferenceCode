#include "MainTitleWidget.h"
#include "BgiLogger.h"
#include "LogoWidget.h"
#include "PromptInfoWidget.h"
#include "DateWidget.h"
#include <QtUiTools/quiloader.h>
#include <QFile>
#include <qwidget.h>
#include <qfileinfo.h>
#include <qlayout.h>
#include "BgiFileTools.h"

MainTitleWidget::MainTitleWidget(QLayout* parent)
    :m_pLayout(parent)
{
    QString data_path = "";
    BgiFileTools::GetDataDirPath(data_path);
    QString fileName = data_path + "//Data//Ui//MainTitle.ui";
    QFileInfo info(fileName);
    if (info.exists())
    {
        QUiLoader  uiLoader;
        QFile file(fileName);
        if (&file != NULL)
        {
            m_pMainTitleWidget = uiLoader.load(&file);
            BGI_ASSERT(m_pMainTitleWidget != NULL);
            if (m_pMainTitleWidget != NULL)
            {
                m_pLayout->addWidget(m_pMainTitleWidget);

                QRect area = QRect(0,0,1920,100);

                QWidget* logo_widget = m_pMainTitleWidget->findChild<QWidget*>("widget_bgiLogo");
                /*logo_widget->move(0, 0);
                logo_widget->resize(area.width() / 6, area.height());*/
                m_spLogoWidget.create(new LogoWidget(logo_widget));
                

                QWidget* prompt_info_widget = m_pMainTitleWidget->findChild<QWidget*>("widget_promptInfo");
                /*prompt_info_widget->move(area.width() / 6, 0);
                prompt_info_widget->resize(area.width() * 3 / 4, area.height());*/
                m_spPromptInfoWidget.create(new PromptInfoWidget(prompt_info_widget));
                

                QWidget* date_widget = m_pMainTitleWidget->findChild<QWidget*>("widget_date");
				/* date_widget->move(area.width() * 5 / 6, 0);
				 date_widget->resize(area.width() / 6, area.height());*/
                m_spDateWidget.create(new DateWidget(date_widget));
                

                m_pMainTitleWidget->show();

                m_button_about = m_pMainTitleWidget->findChild<QPushButton*>("pushButton_about");
                connect(m_button_about,&QPushButton::clicked, this, [=]{emit SignalClickAbout(); });
            }
            
        }
        else
        {
            BGI_ASSERT(false);
        }
    }
    else
    {
        BGI_ASSERT(false);
    }
    
    
}

MainTitleWidget::~MainTitleWidget()
{

}

void MainTitleWidget::Show(bool bShow)
{
    if (m_pMainTitleWidget)
        return;
    if (bShow)
    {
        m_pMainTitleWidget->show();

    }    
    else
    {
        m_pMainTitleWidget->hide();
    }
}

void MainTitleWidget::SlotChangeAboutButtonText(QString strText)
{
    m_button_about->setText(strText);
}