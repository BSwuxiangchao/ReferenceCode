/*
* @Abrief 日期时间
* @note 日期时间窗口的实现
* @version 1.0
* @author mosongjin 2021-09-29
*/


#ifndef _DATE_WIDGET_H
#define _DATE_WIDGET_H

#include <qwidget.h>
#include <qlabel.h>
#include <qobject.h>

class DateWidget : public QObject
{
    Q_OBJECT
public:
    DateWidget(QWidget* parent);
    virtual ~DateWidget();

private slots:
    void UpdateTime();
private:
    QWidget* m_pParentWidget;

    QLabel* m_pVersionLabel;
    QLabel* m_pOpenTimeLabel;
    QLabel* m_pDateLabel;
};

#endif // MAIN_TITLE_WIDGET_H
