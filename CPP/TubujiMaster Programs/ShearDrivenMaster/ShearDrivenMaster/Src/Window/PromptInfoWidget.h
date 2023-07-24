/*
* @Abrief 提示信息
* @note 提示窗口的实现
* @version 1.0
* @author mosongjin 2021-09-29
*/


#ifndef PROMPT_INFO_WIDGET_H
#define PROMPT_INFO_WIDGET_H

#include <qwidget.h>
#include <qlabel.h>

class PromptInfoWidget
{
public:
    PromptInfoWidget(QWidget* parent);
    virtual ~PromptInfoWidget();

private:
    QWidget* m_pParentWidget;

    QLabel* m_pPromptInfoLabel;
};

#endif // MAIN_TITLE_WIDGET_H
