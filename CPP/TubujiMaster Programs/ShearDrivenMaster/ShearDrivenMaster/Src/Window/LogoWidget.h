/*
* @Abrief Bgi Logo
* @note Logo窗口的实现
* @version 1.0
* @author mosongjin 2021-09-29
*/


#ifndef LOGO_TITLE_WIDGET_H
#define LOGO_TITLE_WIDGET_H

#include <qwidget.h>
#include <qlabel.h>

class LogoWidget
{
public:
    LogoWidget(QWidget* parent);
    virtual ~LogoWidget();

private:
    QWidget* m_pParentWidget;

    QLabel* m_pLogoLabel;
};

#endif // MAIN_TITLE_WIDGET_H
