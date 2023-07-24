 /*
 * @Abrief 类或结构体描述：用于提示用户的信息窗口；
 * @note 类或结构体描述：用于提示用户的信息窗口；
 * @version 1.0
 * @author wuxiangchao 2023-2-16
 */
#ifndef INFOPROMPTWIDGET_H
#define INFOPROMPTWIDGET_H

#include <QDialog>

typedef enum _tagBUTTON_STATE
{
    NO_BUTTON = 0,
    SURE = 1,
    CANCEL = 2
}BUTTON_STATE;

namespace Ui {
class InfoPromptWidget;
}

class InfoPromptWidget : public QDialog
{
    Q_OBJECT

public:
    explicit InfoPromptWidget(QWidget *parent = nullptr);
    ~InfoPromptWidget();

/******************************************************************************
 * 功能描述：弹出对话框
 * 时    间：2022/2/21 
 * 入    参：bModal表示模态，true-表示模态，false-非模态；
 * 入    参：设置标题,bColorRed:标题颜色，true-红色，false-蓝色
 * 入    参：title：设置标题,bColorRed:标题颜色，true-红色，false-蓝色
 * 入    参：info：设置提示内容
 * 入    参：iButton：设置按钮的显示,1-只显示确定按钮，2-只显示取消，3-显示确定和取消按钮，4-都不显示确定和取消，其他值默认只显示确认按钮
 * 返    回：BUTTON_STATE: 按钮类型；
 ******************************************************************************/
    BUTTON_STATE Show(bool bModal, const QString& title, bool bColorRed, const QString& info, const int& iButton);
    BUTTON_STATE Show(const QString& title, const QString& info, bool bModal = true, const int& iButton = 1, const QColor& colTitleForeground = Qt::black, const QColor& colTitleBackground = Qt::blue);
    BUTTON_STATE Show(const QString& title, const QString& info, const QString& buttonName, const bool& bEnable, bool bModal = true,  const QColor& colTitleForeground = Qt::black, const QColor& colTitleBackground = Qt::blue);

    // 显示对话框:bModal表示模态，true-表示模态，false-非模态
    void DisplayDialog(bool bModal);

    // 设置标题,bColorRed:标题颜色，true-红色，false-蓝色
    void SetTitle(const QString& title, bool bColorRed = false);

/******************************************************************************
 * 功能描述：设置窗体标题
 * 时    间：2022/2/21 
 * 入    参：titleText：标题名称；
 * 入    参：colForeground：标题前景色；
 * 入    参：colBackground：标题背景色；
 * 出    参：无
 * 返    回：无
 ******************************************************************************/
	void SetTitle(const QString& titleText, const QColor& colForeground, const QColor& colBackground);

    // 设置提示内容
    void SetPromptInfo(const QString& info);
    // 设置按钮的显示,1-只显示确定按钮，2-只显示取消，3-显示确定和取消按钮，4-都不显示确定和取消，其他值默认只显示确认按钮
    void DisplayButton(const int& bSure);
    //更改按钮的文本和使能状态
    void DisplayButton(const QString& sTask, const bool& bEnable);

    // 获取按钮状态
    const BUTTON_STATE& GetButtonState()const;
private slots:
    void on_pushButtonSure_clicked();

    void on_pushButtonCancel_clicked();

    void closeEvent(QCloseEvent* event);

private:
    // 按钮状态
    BUTTON_STATE m_iButtonState;

private:
    Ui::InfoPromptWidget *ui;
};

#endif // INFOPROMPTWIDGET_H
