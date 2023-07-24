/*
* @Abrief 主标题
* @note 主标题窗口包含logo、提示信息、日期窗口
* @version 1.0
* @author mosongjin 2021-09-29
*/


#ifndef MAIN_TITLE_WIDGET_H
#define MAIN_TITLE_WIDGET_H

#include <qsharedpointer.h>
#include <qobject.h>
#include <QPushButton>

class QWidget;
class QLayout;

class LogoWidget;
class PromptInfoWidget;
class DateWidget;

class MainTitleWidget : public QObject
{
    Q_OBJECT
public:
    MainTitleWidget(QLayout* parent);
    virtual ~MainTitleWidget();

    void Show(bool bShow);
signals:
    void SignalClickAbout();

public slots:
    void SlotChangeAboutButtonText(QString strText);
private:
    QWidget* m_pMainTitleWidget;
    QLayout* m_pLayout;

    QSharedPointer<LogoWidget*> m_spLogoWidget;
    QSharedPointer<PromptInfoWidget*> m_spPromptInfoWidget;
    QSharedPointer<DateWidget*> m_spDateWidget;
    QPushButton* m_button_about;
};

#endif // MAIN_TITLE_WIDGET_H
